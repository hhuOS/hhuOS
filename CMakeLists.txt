CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(hhuOS)

INCLUDE_DIRECTORIES(./src/os)

SET(CMAKE_CXX_STANDARD 11)

FILE(GLOB_RECURSE SOURCE_FILES "src/os/*.c" "src/os/*.cpp" "src/os/*.h" "src/os/*.asm" "src/modules/*.c" "src/modules/*.cpp" "src/modules/*.h" "src/modules/*.asm" "Makefile")

ADD_EXECUTABLE(hhuOS ${SOURCE_FILES} src/os/kernel/memory/manager/DefaultBitmapMemoryManager.cpp src/os/kernel/memory/manager/DefaultBitmapMemoryManager.h)

ADD_CUSTOM_TARGET(
        kvm
        COMMAND make kvm
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/os
)
