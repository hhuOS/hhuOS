cmake_minimum_required(VERSION 3.14)

project(lib.usb)
message(STATUS "Project" ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

add_subdirectory(input)
add_subdirectory(io_control)
add_subdirectory(sound)