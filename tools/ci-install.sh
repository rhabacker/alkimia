#!/bin/bash

# SPDX-FileCopyrightText: 2015-2016 Collabora Ltd.
# SPDX-FileCopyrightText: 2020 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: MIT

set -euo pipefail
set -x

# ci_distro:
# OS distribution in which we are testing
# Typical values: auto opensuse ubuntu
: "${ci_distro:=auto}"

# ci_host:
# the host to build for
: "${ci_host:=native}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"

r=$(realpath $0)
r=$(dirname $r)

if [ -f /.dockerenv ]; then
    sudo=
fi

. $r/ci-lib.sh

distros=(opensuse_tumbleweed opensuse_leap debian)
variants=(kf5 kf5_webengine kf5_webkit kde4)
hosts=(native mingw32 mingw64)

repos_opensuse_leap=(
    https://download.opensuse.org/repositories/windows:/mingw/$repo/windows:mingw.repo
)
repos_opensuse_leap_mingw32=(
    https://download.opensuse.org/repositories/windows:/mingw:/win32/$repo/windows:mingw:win32.repo
)
repos_opensuse_leap_mingw64=(
    https://download.opensuse.org/repositories/windows:/mingw:/win64/$repo/windows:mingw:win64.repo
)
source_packages_opensuse_leap=()
source_packages_opensuse_leap_native=(libalkimia)
source_packages_opensuse_leap_mingw32=(mingw32-libalkimia)
source_packages_opensuse_leap_mingw64=(mingw64-libalkimia)

packages_opensuse_leap=(
    cmake
    # xvfb-run does not have added all required tools
    openbox
    psmisc # killall
    shadow # useradd
    sharutils # uuencode
    sudo # sudoers
    xvfb-run
    which
    xauth
    xterm
    # for screenshots
    xwd ImageMagick
)

packages_opensuse_leap_native_kde4=(
    # for libQtWebKit-devel
    gcc-c++
    extra-cmake-modules
    libkde4-devel
    libQtWebKit-devel
    kdebase4-runtime
    gmp-devel
)
packages_opensuse_leap_mingw32_kde4=(
    ${prefix}-cross-gcc-c++
    ${prefix}-extra-cmake-modules
    ${prefix}-libkde4-devel
    ${prefix}-gmp-devel
)
packages_opensuse_leap_mingw64_kde4=${packages_opensuse_leap_mingw32_kde4[@]}

check_config
add_repos
add_source_packages
add_packages
add_user
