cmake_minimum_required(VERSION 3.10)
project(demo)
set(CMAKE_CXX_STANDARD 17)

include(../examples.cmake.in)

include_directories(../../src)

find_library(teensy_ili9341_controls NAME teensy_ili9341_controls)
include_directories(${teensy_ili9341_controls_INCLUDE_DIR})

add_definitions(-fms-extensions)
add_definitions(-DARDUINO=146)

add_executable(demo basic.cpp)

target_link_libraries(demo teensy_x86_stubs)
target_link_libraries(demo teensy_x86_bounce2_stubs)
target_link_libraries(demo teensy_x86_encoder_stubs)
target_link_libraries(demo teensy_st7735_linux_stubs)
target_link_libraries(demo teensy_st7735_linux_opengl)
target_link_libraries(demo teensy_gfx)

include(../../cmake/opengl.cmake)
target_link_libraries(demo ${LIBS})

if  (APPLE)
        target_link_libraries(demo
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()