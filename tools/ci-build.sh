#!/bin/bash

# Copyright © 2015-2016 Collabora Ltd.
# Copyright © 2020 Ralf Habacker <ralf.habacker@freenet.de>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

set -euo pipefail
set -x

##
## initialize support to run cross compiled executables
##
# syntax: init_wine <path1> [<path2> ... [<pathn>]]
# @param  path1..n  pathes for adding to wine executable search path
#
# The function exits the shell script in case of errors
#
init_wine() {
    if ! command -v wineboot >/dev/null; then
        echo "wineboot not found"
        exit 1
    fi

    # run without X11 display to avoid that wineboot shows dialogs
    wineboot -fi

    # add local paths to wine user path
    local addpath="" d="" i
    for i in "$@"; do
        local wb=$(winepath -w "$i")
        addpath="$addpath$d$wb"
        d=";"
    done

    # create registry file from template
    local wineaddpath=$(echo "$addpath" | sed 's,\\,\\\\\\\\,g')
    r=$(realpath $0)
    r=$(dirname $r)
    sed "s,@PATH@,$wineaddpath,g" "$r/user-path.reg.in" > user-path.reg

    # add path to registry
    wine regedit /C user-path.reg

    # check if path(s) has been set and break if not
    local o=$(wine cmd /C "echo %PATH%")
    case "$o" in
        (*z:* | *Z:*)
            # OK
            ;;
        (*)
            echo "Failed to add Unix paths '$*' to path: Wine %PATH% = $o" >&2
            exit 1
            ;;
    esac
}

#
# prepare running cross compile executables
#
# @param binpath  path for executables in build root
#
init_cross_runtime() {
    # CFLAGS and CXXFLAGS does do work, checked with cmake 3.15
    export LDFLAGS="-${ci_runtime}-libgcc"
    # enable tests if supported
    if [ "$ci_test" = yes ]; then
        sysroot=$("${ci_host}-gcc" --print-sysroot)
        # check if the prefix is a subdir of sysroot (e.g. openSUSE)
        if [ -d "${sysroot}/${ci_host}" ]; then
            dep_prefix="${sysroot}/${ci_host}"
        else
            # fallback: assume the dependency libraries were built with --prefix=/${ci_host}
            dep_prefix="/${ci_host}"
        fi
        # choose correct wine architecture
        if [ "${ci_host%%-*}" = x86_64 ]; then
            export WINEARCH=win64
            export WINEPREFIX=${HOME}/.wine64
        else
            export WINEARCH=win32
            export WINEPREFIX=${HOME}/.wine32
        fi
        # clean wine prefix
        rm -rf ${WINEPREFIX}
        libgcc_path=
        if [ "$ci_runtime" = "shared" ]; then
            libgcc_path=$(dirname "$("${ci_host}-gcc" -print-libgcc-file-name)")
        fi
        init_wine "${dep_prefix}/bin" "$1" ${libgcc_path:+"$libgcc_path"}
    fi
}

# ci_host:
# See ci-install.sh
: "${ci_host:=native}"

# ci_jobs:
# number of jobs
: "${ci_jobs:=5}"

# ci_runtime:
# One of static, shared; used for windows cross builds
: "${ci_runtime:=shared}"

# ci_test:
# If yes, run tests; if no, just build
: "${ci_test:=yes}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"


# specify build dir
builddir=ci-build-${ci_variant}-${ci_host}
srcdir="$(pwd)"

# start dbus session, which is required by kio
if ! test -v DBUS_SESSION_BUS_PID || test -z "$DBUS_SESSION_BUS_PID"; then
    eval `dbus-launch --sh-syntax`
fi

# settings for build variants
case "$ci_variant" in
    (kf5)
        cmake_options="-DBUILD_APPLETS=0 -DBUILD_TESTING=1"
        export QT_LOGGING_RULES="*=true"
        start_kde_session=kdeinit5
        cmake_suffix="kf5"
        ;;

    (kde4)
        cmake_options="-DBUILD_QT4=1 -DKDE4_BUILD_TESTS=1"
        start_kde_session=kdeinit4
        cmake_suffix="kde4"
        ;;
esac

# settings for platforms
case "$ci_host" in
    (i686-w64-mingw32)
        cmake="mingw32-cmake-$cmake_suffix"
        init_cross_runtime $builddir/bin
        start_kde_session="wine $start_kde_session"

        ;;
    (x86_64-w64-mingw32)
        cmake="mingw64-cmake-$cmake_suffix"
        init_cross_runtime $builddir/bin
        start_kde_session="wine $start_kde_session"
        ;;
    (*)
        cmake="cmake-$cmake_suffix"
        ;;
esac

# create subdirs
rm -rf ${builddir}
mkdir -p ${builddir}
cd ${builddir}

# configure project
$cmake -- $cmake_options ..
make -j$ci_jobs

# start xvfb session - will restart in case of crashes
(while ! test -f ./finished; do xvfb-run -s "+extension GLX +render" -a -n 99 openbox; done) &
export DISPLAY=:99

# start kde session
$start_kde_session

# run tests
ctest --output-on-failure --timeout 60 --jobs $ci_jobs
#ctest -VV --timeout 60

# show screenshot in case of errors
if test $? -ne 0; then
    xwd -root -silent | convert xwd:- png:/tmp/screenshot.png
    cat /tmp/screenshot.png | uuencode screenshot
fi

# run install
make install DESTDIR=$PWD/tmp

# kill x session
touch finished
killall -s 9 $start_kde_session openbox dbus-daemon || true
