cmake_minimum_required(VERSION 3.10)
project(example_menu)

set(CMAKE_CXX_STANDARD 17)

include(../examples.cmake.in)
include_directories(../../src)

add_executable(example_menu menus.cpp)

target_link_libraries(example_menu teensy_x86_stubs)
target_link_libraries(example_menu teensy_x86_sd_stubs)
target_link_libraries(example_menu teensy_x86_bounce2_stubs)
target_link_libraries(example_menu teensy_x86_encoder_stubs)
target_link_libraries(example_menu teensy_audio_x86_stubs)
target_link_libraries(example_menu teensy_audio_x86_stubs_soundio)
target_link_libraries(example_menu rtmidi)
target_link_libraries(example_menu teensy_x86_midi_stubs)
target_link_libraries(example_menu teensy_st7735_linux_stubs)
target_link_libraries(example_menu teensy_st7735_linux_opengl)
target_link_libraries(example_menu teensy_x86_bounce2_stubs)
target_link_libraries(example_menu teensy_gfx)
target_link_libraries(example_menu teensy_gfx_controls)

include(../../cmake/opengl.cmake)
target_link_libraries(example_menu ${LIBS})

target_link_libraries(example_menu "-L/usr/local/lib -lsoundio")
if  (APPLE)
        target_link_libraries(example_menu
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()