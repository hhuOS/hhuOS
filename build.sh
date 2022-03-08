#!/bin/bash

# Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
# Institute of Computer Science, Department Operating Systems
# Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
#
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

TARGET="towboot"
BUILD_DIR="build"
CORE_COUNT=$(nproc)
VALID_TARGETS="grub towboot"
FORBIDDEN_DIR_NAMES="cmake loader media src"

parse_target() {
    local target=$1
    
    for name in ${VALID_TARGETS}; do
        if [ "${target}" == ${name} ]; then
            TARGET=${target}
            return
        fi
    done
    
    printf "Invalid target '%s'!" "${target}"
    exit 1
}

parse_directory() {
    local directory=$1

    # check if dir-name is already used by this project
    for name in $FORBIDDEN_DIR_NAMES; do
        if [[ "${directory}" == *"${name}"* ]]; then
            printf "Please use another name than '%s' for the build directory!\n" "${name}"
            exit 1
        fi
    done

    BUILD_DIR=${directory}

    # add name of directory to local git ignore list
    if [ ! "${directory}" == "build" ] && [ "$(grep -w ${directory} .git/info/exclude)" == "" ]; then
        echo "${directory}/" >> .git/info/exclude
        printf "Added '%s' to .git/info/exclude\n" "${directory}"
    fi

    if [ ! -f .builddirs ] && [ ! "${directory}" == "build" ]; then
        touch .builddirs
    fi

    # add name of directory to list of used build-dirs
    if [ ! "${directory}" == "build" ] && [ "$(grep -w ${directory} .builddirs)" == "" ]; then
        echo "${directory}" >> .builddirs
        printf "Added '%s' to .builddirs\n" "${directory}"
    fi
}

parse_ncores() {
    local ncores=$1

    if [ ${ncores} != 0 ]; then
        CORE_COUNT=${ncores}
    fi
}

cleanup() {
    if [ -f hhuOS.iso ]; then
        printf "Removing 'hhuOS.iso'\\n"
        rm hhuOS.iso
    fi
    
    if [ -f hhuOS.img ]; then
        printf "Removing 'hhuOS.img'\\n"
        rm hhuOS.img
    fi
    
    if [ -f loader/grub/boot/hhuOS.bin ]; then
        printf "Removing 'loader/grub/boot/hhuOS.bin'\\n"
        rm loader/grub/boot/hhuOS.bin
    fi
    
    if [ -f loader/grub/boot/hhuOS.initrd ]; then
        printf "Removing 'loader/grub/boot/hhuOS.initrd'\\n"
        rm loader/grub/boot/hhuOS.initrd
    fi
    
    if [ -f loader/towboot/hhuOS.bin ]; then
        printf "Removing 'loader/towboot/hhuOS.bin'\\n"
        rm loader/towboot/hhuOS.bin
    fi
    
    if [ -f loader/towboot/hhuOS.initrd ]; then
        printf "Removing 'loader/towboot/hhuOS.initrd'\\n"
        rm loader/towboot/hhuOS.initrd
    fi
    
    if [ -f loader/towboot/towboot-ia32.efi ]; then
        printf "Removing 'loader/towboot/towboot-ia32.efi'\\n"
        rm loader/towboot/towboot-ia32.efi
    fi
    
    if [ -f loader/towboot/towboot-x64.efi ]; then
        printf "Removing 'loader/towboot/towboot-x64.efi'\\n"
        rm loader/towboot/towboot-x64.efi
    fi
    
    if [ -d initrd/bin/ ]; then
        printf "Removing 'initrd/bin/'\\n"
        rm -r initrd/bin/
    fi

    local builddirs="";

    # get all names of used build directories
    if [ -f .builddirs ]; then
        builddirs=$(cat .builddirs)
    fi

    # remove build directories and remove their names from .git/info/exclude
    for dir in ${builddirs}; do
        if [ "$(echo ${FORBIDDEN_DIR_NAMES} | grep -w ${dir})" == "" ] && [ -d $dir ]; then
            printf "Removing build directory '${dir}'\n"
            rm -r $dir
        fi

        if [ ! "$(grep -w ${dir}/ .git/info/exclude)" == "" ]; then
            grep -vwE "${dir}/" .git/info/exclude > .git/info/tmp && mv .git/info/tmp .git/info/exclude
        fi
    done

    # remove default directory
    if [ -d build ]; then
        printf "Removing build directory 'build'\n"
        rm -r "build"
    fi

    # remove list of build directories
    if [ -f .builddirs ]; then
        rm .builddirs
    fi
}

print_usage() {
    printf "Usage: ./build.sh [OPTION...]
    Available options:
    -t, --target
        Set the the build target (grub/towboot, default: towboot)
    -d, --directory
        Set the build directory.
    -n, --ncore
        Set the number of cores used by make (default: Output of nproc).
    -c, --clean
        Removes all former build directories and removes their names (except build) from .git/info/exclude
        Finishes execution.
    -h, --help
        Show this help message.\\n"
}

parse_args() {
    while [ "${1}" != "" ]; do
        local arg=$1
        local val=$2

        case $arg in
            -t|--target)
            parse_target $val
            ;;
            -d|--directory)
            parse_directory $val
            ;;
            -n|--ncores)
            parse_ncores $val
            ;;
            -c|--clean)
            cleanup
            exit 0
            ;;
            -h|--help)
            print_usage
            exit 0
            ;;
            *)
            printf "Unknown option '%s'\\n" "${arg}"
            print_usage
            exit 1
            ;;
        esac
        shift 2
    done
}

notify() {
  if [ "$1" -eq "0" ]; then ./rocket notify build-success; else ./rocket notify build-failure; fi
}

build() {
    echo "Creating build-directory ${BUILD_DIR}"
    mkdir -p ${BUILD_DIR}
    cd $BUILD_DIR || exit

    echo "Using ${CORE_COUNT} CPU-Cores for make"

    cmake ..
    make -j ${CORE_COUNT} ${TARGET}
    exit_code=$?

    cd ..

    if [ "${CI}" = "true" ]; then
      notify "$exit_code"
    fi

    exit "$exit_code";
}

parse_args "$@"

build


