#!/bin/bash

# Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

BUILD_TYPE="Default"
TARGET="towboot"
BUILD_DIR="build"
CORE_COUNT=$(nproc)
VALID_TARGETS="grub limine towboot"
FORBIDDEN_DIR_NAMES="cmake loader media src"

parse_target() {
    local target=$1
    
    for name in ${VALID_TARGETS}; do
        if [ "${target}" == ${name} ]; then
            TARGET=${target}
            return
        fi
    done
    
    printf "Invalid target '%s'!\\n" "${target}"
    exit 1
}

parse_directory() {
    local directory=$1

    # Check if dir-name is already used by this project
    for name in $FORBIDDEN_DIR_NAMES; do
        if [[ "${directory}" == *"${name}"* ]]; then
            printf "Please use another name than '%s' for the build directory!\n" "${name}"
            exit 1
        fi
    done

    BUILD_DIR=${directory}

    # Add name of directory to local git ignore list
    if [ ! "${directory}" == "build" ] && [ "$(grep -w ${directory} .git/info/exclude)" == "" ]; then
        echo "${directory}/" >> .git/info/exclude
        printf "Added '%s' to .git/info/exclude\n" "${directory}"
    fi

    if [ ! -f .builddirs ] && [ ! "${directory}" == "build" ]; then
        touch .builddirs
    fi

    # Add name of directory to list of used build-dirs
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

parse_build_type() {
    local debug=$1

    BUILD_TYPE="${debug}"
}

remove() {
    local path=$1

    if [ -f "${path}" ]; then
        printf "Removing '%s'\\n" "${path}"
        rm -f "${path}"
    elif [ -d "${path}" ]; then
        printf "Removing '%s'\\n" "${path}"
        rm -rf "${path}"
    fi
}

cleanup() {
    remove "hhuOS-grub.iso"
    remove "hhuOS-grub-vdd.iso"
    remove "hhuOS-limine.iso"
    remove "hhuOS-limine-vdd.iso"
    remove "hhuOS-towboot.img"
    remove "hhuOS-towboot-vdd.img"
    remove "floppy0.img"
    remove "hdd0.img"
    remove "loader/grub/iso/boot/hhuOS.bin"
    remove "loader/grub/iso/boot/hdd0.img"
    remove "loader/limine/limine"
    remove "loader/limine/iso/hhuOS.bin"
    remove "loader/limine/iso/hdd0.img"
    remove "loader/limine/iso/limine-uefi-cd.bin"
    remove "loader/limine/iso/limine-bios-cd.bin"
    remove "loader/limine/iso/limine-bios.sys"
    remove "loader/limine/iso/EFI"
    remove "loader/towboot/hhuOS.bin"
    remove "loader/towboot/hdd0.img"
    remove "loader/towboot/towboot-ia32.efi"
    remove "loader/towboot/towboot-x64.efi"
    remove "initrd/asciimation/"
    remove "initrd/beep/"
    remove "initrd/bin/"
    remove "floppy0/img/bin/"
    remove "floppy0/img/books/"
    remove "hdd0/img/bin/"
    remove "hdd0/img/user/asciimation"
    remove "hdd0/img/user/beep"
    remove "hdd0/img/user/books"
    remove "hdd0/img/media/"

    local builddirs="";

    # Get all names of used build directories
    if [ -f .builddirs ]; then
        builddirs=$(cat .builddirs)
    fi

    # Remove build directories and remove their names from .git/info/exclude
    for dir in ${builddirs}; do
        if [ "$(echo ${FORBIDDEN_DIR_NAMES} | grep -w ${dir})" == "" ] && [ -d $dir ]; then
            remove ${dir}
        fi

        if [ ! "$(grep -w ${dir}/ .git/info/exclude)" == "" ]; then
            grep -vwE "${dir}/" .git/info/exclude > .git/info/tmp && mv .git/info/tmp .git/info/exclude
        fi
    done

    # Remove default directory
    remove "build"
    if [ -d build ]; then
        printf "Removing build directory 'build'\n"
        rm -r "build"
    fi

    # Remove list of build directories
      remove .builddirs
}

print_usage() {
    printf "Usage: ./build.sh [OPTION...]
    Available options:
    -t, --target
        Set the the build target (grub/towboot, default: towboot).
    -d, --directory
        Set the build directory.
    -g, --type
        Set the build type (Default/Debug).
    -n, --ncore
        Set the number of cores used by make (default: Output of nproc).
    -c, --clean
        Removes all former build directories and removes their names (except build) from .git/info/exclude
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
            -g|--type)
            parse_build_type $val
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

build() {
    echo "Creating build-directory ${BUILD_DIR}"
    mkdir -p ${BUILD_DIR}
    cd $BUILD_DIR || exit

    echo "Using ${CORE_COUNT} CPU-Cores for make"

    cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    make -j "${CORE_COUNT}" "${TARGET}"
}

parse_args "$@"

build