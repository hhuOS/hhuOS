CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(hhuOS)

INCLUDE_DIRECTORIES(./src/os ./src/os/kernel/acpi/include)

SET(CMAKE_CXX_STANDARD 11)

FILE(GLOB_RECURSE SOURCE_FILES "src/os/*.c" "src/os/*.cc" "src/os/*.h" "src/os/*.asm" "src/modules/*.c" "src/modules/*.cc" "src/modules/*.h" "src/modules/*.asm" "Makefile")

ADD_EXECUTABLE(hhuOS ${SOURCE_FILES} src/os/filesystem/RamFs/graphics/GraphicsDeviceNameNode.h src/os/filesystem/RamFs/graphics/GraphicsDeviceNameNode.cc src/os/filesystem/RamFs/graphics/GraphicsResolutionsNode.h src/os/filesystem/RamFs/graphics/GraphicsResolutionsNode.cc src/os/filesystem/RamFs/graphics/GraphicsMemoryNode.h src/os/filesystem/RamFs/graphics/GraphicsMemoryNode.cc)

ADD_CUSTOM_TARGET(
        kvm
        COMMAND make kvm
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/os
)
