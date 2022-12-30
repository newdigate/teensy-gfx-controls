cmake_minimum_required(VERSION 3.10)
project(teensy-gfx-controls C CXX)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
		teensy_controls.cpp
		)

set(HEADER_FILES
		teensy_controls.h
		scenecontroller.h
		VirtualView.h
		)

IF (DEFINED BUILD_FOR_TEENSY)
    include(CMakeLists.teensy.cmake.in)
ELSE()
    include(CMakeLists.linux.cmake.in)
ENDIF()