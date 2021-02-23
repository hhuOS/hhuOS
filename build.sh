#!/bin/bash

CORE_COUNT=$(nproc)
BUILD_DIR=build
FORBIDDEN_DIR_NAMES="cmake hdd initrd loader media music src tool"

parse_ncores() {
    local ncores=$1

    if [ ! $ncores == 0 ]; then
        CORE_COUNT=$ncores
    fi
}

parse_direcory() {
    local directory=$1

    # check if dir-name is already used by this project
    for name in $FORBIDDEN_DIR_NAMES; do
        if [ "${directory}" == $name ]; then
            printf "Please use another name than '%s' for the build-directory!\n" "${name}"
            exit 1
        fi
    done

    BUILD_DIR=$directory

    # add name of directory to local git ignore list
    if [ ! "${directory}" == "build" ] && [ "$(grep -w ${directory} .git/info/exclude)" == "" ]; then
        echo "${directory}/" >> .git/info/exclude
        printf "[INFO] Added '%s' to .git/info/exclude\n" "${directory}"
    fi

    if [ ! -f .builddirs ] && [ ! "${directory}" == "build" ]; then
        touch .builddirs
    fi

    # add name of directory to list of used build-dirs
    if [ ! "${directory}" == "build" ] && [ "$(grep -w ${directory} .builddirs)" == "" ]; then
        echo "${directory}" >> .builddirs
        printf "[INFO] Added '%s' to .builddirs\n" "${directory}"
    fi
}

cleanup() {

    # remove iso and img files
    if [ -f hhuOS.iso ]; then
        rm hhuOS.iso
    fi

    if [ -f hdd0.img ]; then
        rm hdd0.img
    fi
    if [ -f eth0.dump ]; then
        rm eth0.dump
    fi

    local builddirs="";

    # get all names of used build directories
    if [ -f .builddirs ]; then
        builddirs=$(cat .builddirs)
    fi

    # remove build directories and remove their names from .git/info/exclude
    for dir in $builddirs; do
        if [ "$(echo $FORBIDDEN_DIR_NAMES | grep -w ${dir})" == "" ] && [ -d $dir ]; then
            printf "Remove build directory '${dir}'\n"
            rm -r $dir
        fi

        if [ ! "$(grep -w ${dir}/ .git/info/exclude)" == "" ]; then
            grep -vwE "${dir}/" .git/info/exclude > .git/info/tmp && mv .git/info/tmp .git/info/exclude
        fi
    done

    # remove default directory
    if [ -d build ]; then
        printf "Remove build directory 'build'\n"
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
            -n|--ncores)
            parse_ncores $val
            ;;
            -d|--directory)
            parse_direcory $val
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
    echo "[INFO] Create build-directory $BUILD_DIR"
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR || exit

    echo "[INFO] Use $CORE_COUNT CPU-Cores for make"

    cmake ..
    make -j$CORE_COUNT iso
}

parse_args "$@"

build


