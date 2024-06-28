cmake_minimum_required(VERSION 3.14)

project(playusb)
message(STATUS "Project" ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

# Set source files
set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/usb/UsbAudio.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.sound lib.user.time lib.user.graphic lib.usb)