cmake_minimum_required(VERSION 3.10)
project(demo)
set(CMAKE_CXX_STANDARD 17)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_encoder_stubs)
include_directories(${teensy_x86_encoder_stubs_INCLUDE_DIR})

find_package(teensy_x86_bounce2_stubs)
include_directories(${teensy_x86_bounce2_stubs_INCLUDE_DIR})

find_package(teensy_st7735_linux_stubs)
include_directories(${teensy_st7735_linux_stubs_INCLUDE_DIR})

find_package(teensy_gfx)
include_directories(${teensy_gfx_INCLUDE_DIR})

find_package(teensy_st7735_linux_opengl)
include_directories(${teensy_st7735_linux_extras_opengl_INCLUDE_DIR})

#find_package(teensy_gfx_controls)
#include_directories(${teensy_gfx_controls_INCLUDE_DIR})
include_directories(../../src)

find_package(teensy_ili9341_controls)
include_directories(${teensy_ili9341_controls_INCLUDE_DIR})

add_definitions(-fms-extensions)
add_definitions(-DARDUINO=146)

add_executable(demo basic.cpp)

target_link_libraries(demo ${teensy_x86_stubs_LIBS})
target_link_libraries(demo ${teensy_x86_bounce2_stubs_LIBS})
target_link_libraries(demo ${teensy_x86_encoder_stubs_LIBS})
target_link_libraries(demo ${teensy_st7735_linux_stubs_LIBS})
target_link_libraries(demo ${teensy_st7735_linux_extras_opengl_LIBS})
target_link_libraries(demo ${teensy_gfx_LIBS})

include(../../cmake/opengl.cmake)
target_link_libraries(demo ${LIBS})

if  (APPLE)
        target_link_libraries(demo
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()