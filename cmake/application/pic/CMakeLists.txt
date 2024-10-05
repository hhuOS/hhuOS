cmake_minimum_required(VERSION 3.17)

project(pic)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

# Set source files
set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/pic/Pic.cpp
        ${HHUOS_SRC_DIR}/application/pic/pic_copy.cpp
        ${HHUOS_SRC_DIR}/application/pic/Renderer.cpp
        ${HHUOS_SRC_DIR}/application/pic/DataWrapper.cpp
        ${HHUOS_SRC_DIR}/application/pic/Layer.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base)
