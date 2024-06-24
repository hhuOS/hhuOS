cmake_minimum_required(VERSION 3.14)

project(msd)
message(STATUS "Project" ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

set(SOURCE_FILES
    ${HHUOS_SRC_DIR}/application/usb/UsbMsd.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.crt0 lib.user.time lib.user.async lib.user.base lib.usb)