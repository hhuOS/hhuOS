#!/bin/bash

# Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

readonly VALID_BUILD_TYPES=("Debug" "Default" "Release" "RelWithDebInfo" "MinSizeRel")
readonly VALID_TARGETS=("grub" "limine" "towboot")
readonly VALID_GENERATORS=("Unix Makefiles" "Ninja")

BUILD_TYPE="Release"
TARGET="towboot"

if command -v ninja > /dev/null 2>&1; then
  GENERATOR="Ninja"
else
  GENERATOR="Unix Makefiles"
fi

parse_target() {
    local target="${1}"
    
    for name in "${VALID_TARGETS[@]}"; do
        if [ "${target}" == "${name}" ]; then
            TARGET="${target}"
            return
        fi
    done
    
    printf "Invalid target '%s'!\\n" "${target}"
    exit 1
}

parse_build_type() {
    local build_type="${1}"

    for name in "${VALID_BUILD_TYPES[@]}"; do
        if [ "${build_type}" == "${name}" ]; then
            BUILD_TYPE="${build_type}"
            return
        fi
    done

    printf "Invalid build type '%s'!\\n" "${build_type}"
    exit 1
}

parse_generator() {
    local generator="${1}"

    for name in "${VALID_GENERATORS[@]}"; do
        if [ "${generator}" == "${name}" ]; then
            GENERATOR="${generator}"
            return
        fi
    done

    printf "Invalid generator '%s'!\\n" "${generator}"
    exit 1
}

remove() {
    local path="${1}"

    if [ -f "${path}" ]; then
        printf "Removing '%s'\\n" "${path}"
        rm -f "${path}"
    elif [ -d "${path}" ]; then
        printf "Removing '%s'\\n" "${path}"
        rm -rf "${path}"
    fi
}

cleanup() {
    remove "cmake-build-debug"
    remove "cmake-build-default"
    remove "cmake-build-release"
    remove "cmake-build-relwithdebinfo"
    remove "cmake-build-minsizerel"
    remove "hhuOS-grub.iso"
    remove "hhuOS-grub-vdd.iso"
    remove "hhuOS-limine.iso"
    remove "hhuOS-limine-vdd.iso"
    remove "hhuOS-towboot.img"
    remove "hhuOS-towboot-vdd.img"
    remove "ne2k.dump"
    remove "rtl8139.dump"
    remove "floppy0.img"
    remove "hdd0.img"
    remove "RELEASEIa32_OVMF.fd"
    remove "disk/floppy0/bin/"
    remove "disk/floppy0/books/"
    remove "disk/hdd0/bin/"
    remove "disk/hdd0/user/asciimation"
    remove "disk/hdd0/user/beep"
    remove "disk/hdd0/user/books"
    remove "disk/hdd0/user/doom"
    remove "disk/hdd0/user/gameboy"
    remove "disk/hdd0/user/megadrive"
    remove "disk/hdd0/user/quake"
    remove "disk/hdd0/user/wav"
    remove "disk/hdd0/media/"
    remove "tools/nettest/nettest-server"
}

print_usage() {
    printf "Usage: ./build.sh [OPTION...]
    Available options:
    -t, --target
        Set the the build target (towboot/limine/grub, default: towboot).
    -b, --type
        Set the build type (Debug/Default/Release/RelWithDebInfo/MinSizeRel, default: Release).
    -g, --generator
        Set the generator used by cmake ('Unix Makefiles'/Ninja, default: Unix Makefiles).
    -n, --ncore
        Set the number of cores used by make (default: Output of nproc).
    -c, --clean
        Remove all build files
    -h, --help
        Show this help message.\\n"
}

parse_args() {
    while [ "${1}" != "" ]; do
        local arg="${1}"
        local val="${2}"

        case $arg in
            -t|--target)
            parse_target "${val}"
            ;;
            -b|--type)
            parse_build_type "${val}"
            ;;
            -g|--generator)
            parse_generator "${val}"
            ;;
            -n|--ncores)
            parse_ncores "${val}"
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
    local build_dir="cmake-build-${BUILD_TYPE,,}"

    if [[ ! -d "${build_dir}" ]]; then
        echo "Creating build directory '${build_dir}'"
        mkdir -p "${build_dir}"

        cd "${build_dir}" || exit
        cmake .. -G "${GENERATOR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
        cd .. || exit
    fi

    cd "${build_dir}" || exit

    if [[ -f "build.ninja" ]]; then
        ninja
    elif [[ -f "Makefile" ]]; then
        make -j "$(nproc)" "${TARGET}"
    else
        echo "No build system found in '${build_dir}'! Try a clean build by executing 'build.sh --clean' first."
        exit 1
    fi
}

parse_args "$@"

build
