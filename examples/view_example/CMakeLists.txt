cmake_minimum_required(VERSION 3.10)
project(view_example)
set(CMAKE_CXX_STANDARD 17)
include(../examples.cmake.in)
include_directories(../../src)

add_executable(view_example view_example.cpp)

target_link_libraries(view_example teensy_x86_stubs)
target_link_libraries(view_example teensy_x86_sd_stubs)
target_link_libraries(view_example teensy_x86_bounce2_stubs)
target_link_libraries(view_example teensy_audio_x86_stubs)
target_link_libraries(view_example teensy_audio_x86_stubs_soundio)
target_link_libraries(view_example rtmidi)
target_link_libraries(view_example teensy_x86_midi_stubs)
target_link_libraries(view_example teensy_x86_midi_stubs_rtmidi)
target_link_libraries(view_example teensy_st7735_linux_stubs)
target_link_libraries(view_example teensy_st7735_linux_opengl)
target_link_libraries(view_example teensy_gfx)

include(opengl.cmake)
target_link_libraries(view_example ${LIBS})

target_link_libraries(view_example "-L/usr/local/lib -lsoundio")
if  (APPLE)
        target_link_libraries(view_example
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()