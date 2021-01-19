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

# ci_distro:
# OS distribution in which we are testing
# Typical values: opensuse ubuntu
: "${ci_distro:=opensuse}"

# ci_host:
# host to build for
: "${ci_host:=native}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"

zypper="/usr/bin/zypper --non-interactive"

install=
source_install=

case "$ci_distro" in
    (opensuse)
        $zypper modifyrepo --enable repo-source
        # save time
        #$zypper update
        repo=$(. /etc/os-release; echo $PRETTY_NAME | sed 's, ,_,g')
        packages=(cmake)
        source_packages=()

        # xvfb-run does not have added all required tools
        packages=(
           "${packages[@]}"
            openbox
            psmisc # killall
            sharutils # uuencode
            xvfb-run
            which
            xauth
            xterm
        )

        # for screenshots
        packages=(
           "${packages[@]}"
            xwd ImageMagick
        )
        case "$ci_variant" in
            (kf5)
                source_packages=(
                    "${source_packages[@]}"
                    alkimia
                )
                packages=(
                    "${packages[@]}"
                    kinit
                    libQt5WebKitWidgets-devel
                )
                ;;

            (kde4)
                $zypper ar --refresh --no-gpgcheck \
                    https://download.opensuse.org/repositories/windows:/mingw/$repo/windows:mingw.repo || true
                case "$ci_host" in
                    (i686-w64-mingw32)
                        $zypper ar --refresh --no-gpgcheck \
                            https://download.opensuse.org/repositories/windows:/mingw:/win32/$repo/windows:mingw:win32.repo || true
                        packages=(
                            "${packages[@]}"
                            mingw32-cross-gcc-c++
                            mingw32-extra-cmake-modules
                            mingw32-gmp-devel
                            # fixed with package date >= 2021-*-*
                            mingw32-libgmp10
                            mingw32-libkde4-devel
                            mingw32-kdebase4-runtime
                            wine-binfmt-standalone
                        )
                        ;;
                    (x86_64-w64-mingw32)
                        $zypper ar --refresh --no-gpgcheck \
                            https://download.opensuse.org/repositories/windows:/mingw:/win64/$repo/windows:mingw:win64.repo || true
                        packages=(
                            "${packages[@]}"
                            mingw64-cross-gcc-c++
                            mingw64-extra-cmake-modules
                            mingw64-gmp-devel
                            # fixed with package date >= 2021-*-*
                            mingw32-libgmp10
                            mingw64-libkde4-devel
                            mingw64-kdebase4-runtime
                            wine-binfmt-standalone
                        )
                        ;;
                    (native)
                        # for libQtWebKit-devel
                        $zypper ar --refresh --no-gpgcheck \
                            https://download.opensuse.org/repositories/windows:/mingw/$repo/windows:mingw.repo || true
                        packages=(
                            "${packages[@]}"
                            gcc-c++
                            extra-cmake-modules
                            libkde4-devel
                            libQtWebKit-devel
                            kdebase4-runtime
                            gmp-devel
                        )
                        ;;
                esac
        esac

        # update package repos
        $zypper refresh
        # install source packages
        if test -v "source_packages"; then
            $zypper source-install -d "${source_packages[@]}"
        fi
        # install remaining packages
        $zypper install "${packages[@]}"
        ;;
esac
