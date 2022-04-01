#!/bin/bash

# Copyright © 2022 Ralf Habacker <ralf.habacker@freenet.de>
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

check_config() {
    if [[ ! " ${distros[@]} " =~ "${ci_distro}" ]]; then
        echo "distro $ci_distro not configured"
        exit 1
    fi
    if [[ ! " ${variants[@]} " =~ "${ci_variant}" ]]; then
        echo "variant $ci_variant not configured"
        exit 1
    fi
    if [[ ! " ${hosts[@]} " =~ "${ci_host}" ]]; then
        echo "variant $ci_host not configured"
        exit 1
    fi
}

add_repos_cmd() {
    declare -a argAry1=("${!1}")
    for i in "${argAry1[@]}"; do
        $install_repo "$i" || true
    done
}

install_source_packages_cmd() {
    declare -a argAry1=("${!1}")
    if ((${#argAry1[@]})); then
        $install_source_package $2 "${argAry1[@]}"
        echo $?
    fi
}

install_packages_cmd() {
    declare -a argAry1=("${!1}")
    if ((${#argAry1[@]})); then
        $install_package "${argAry1[@]}"
        echo $?
    fi
}

add_repos() {
    declare -n repos_distro=repos_${ci_distro}
    declare -n repos_distro_host=repos_${ci_distro}_${ci_host}
    declare -n repos_distro_host_variant=repos_${ci_distro}_${ci_host}_${ci_variant}

    repos=()
    if [[ -v repos_distro ]] && ((${#repos_distro[@]})); then
        repos=(${repos[@]} ${repos_distro[@]})
    fi
    if [[ -v repos_distro_host ]] && ((${#repos_distro_host[@]})); then
        repos=(${repos[@]} ${repos_distro_host[@]})
    fi
    if [[ -v repos_distro_host_variant ]] && ((${#repos_distro_host_variant[@]})); then
        repos=(${repos[@]} ${repos_distro_host_variant[@]})
    fi
    echo $repos
    echo repos[@]
    echo ${repos[@]}

    add_repos_cmd repos[@]
}

add_source_packages() {
    # add source packages
    declare -n source_packages_distro=source_packages_${ci_distro}
    declare -n source_packages_distro_host=source_packages_${ci_distro}_${ci_host}
    declare -n source_packages_distro_host_variant=source_packages_${ci_distro}_${ci_host}_${ci_variant}

    source_packages=()
    if [[ -v source_packages_distro ]] && ((${#source_packages_distro[@]})); then
        source_packages=(${source_packages[@]} ${source_packages_distro[@]})
    fi
    if [[ -v source_packages_distro_host ]] && ((${#source_packages_distro_host[@]})); then
        source_packages=(${source_packages[@]} ${source_packages_distro_host[@]})
    fi
    if [[ -v source_packages_distro_host_variant ]] && ((${#source_packages_distro_host_variant[@]})); then
        source_packages=(${source_packages[@]} ${source_packages_distro_host_variant[@]})
    fi

    if ((${#source_packages[@]})); then
        install_source_packages_cmd source_packages[@] -d
    fi
}

add_packages() {
    # add packages
    declare -n packages_distro=packages_${ci_distro}
    declare -n packages_distro_host=packages_${ci_distro}_${ci_host}
    declare -n packages_distro_host_variant=packages_${ci_distro}_${ci_host}_${ci_variant}

    packages=()
    if [[ -v packages_distro ]] && ((${#packages_distro[@]})); then
        packages=(${packages[@]} ${packages_distro[@]})
    fi
    if [[ -v packages_distro_host ]] && ((${#packages_distro_host[@]})); then
        packages=(${packages[@]} ${packages_distro_host[@]})
    fi
    if [[ -v packages_distro_host_variant ]] && ((${#packages_distro_host_variant[@]})); then
        packages=(${packages[@]} ${packages_distro_host_variant[@]})
    fi

    if ((${#packages[@]})); then
        install_packages_cmd packages[@]
    fi
}

add_user() {
    # Add the user that we will use to do the build inside the
    # Docker container, and let them use sudo
    if [ -f /.dockerenv ] && [ -z `getent passwd | grep user` ]; then
        useradd -m user
        passwd -ud user
        echo "user ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/nopasswd
        chmod 0440 /etc/sudoers.d/nopasswd
    fi
}


if [[ ! -v sudo ]]; then
    sudo=sudo
fi
run=
ci_distro=$(. /etc/os-release; echo ${ID} | sed 's,-,_,g')
bits=$(echo $ci_host | sed 's,mingw,,g')
repo=$(. /etc/os-release; echo $PRETTY_NAME | sed 's, ,_,g')
prefix=${ci_host}
install="$run $sudo /usr/bin/zypper --non-interactive"
install_repo="$install ar --refresh --no-gpgcheck"
install_source_package="$install source-install"
install_package="$install install"
