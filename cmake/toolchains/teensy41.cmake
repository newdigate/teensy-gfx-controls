set(TEENSY_VERSION 41 CACHE STRING "Set to the Teensy version corresponding to your board (30 or 31 allowed)" FORCE)
set(CPU_CORE_SPEED 600000000 CACHE STRING "Set to 24000000, 48000000, 72000000 or 96000000 to set CPU core speed" FORCE) # Derived variables
set(CMAKE_EXE_LINKER_FLAGS "--specs=nano.specs" CACHE INTERNAL "")
#teensy compiler options
set(COMPILERPATH "/opt/gcc-arm-none-eabi-10.3-2021.10/bin/")
#set(COMPILERPATH "/Applications/ARM/bin/")

add_definitions(-DTEENSY_VERSION=${TEENSY_VERSION})
include(FetchContent)

FetchContent_Declare(teensy_cores
        GIT_REPOSITORY https://github.com/PaulStoffregen/cores
        GIT_TAG        master
)
FetchContent_MakeAvailable(teensy_cores)

FetchContent_Declare(teensy_cmake_macros
        GIT_REPOSITORY https://github.com/newdigate/teensy-cmake-macros
        GIT_TAG        noinstall
)
FetchContent_MakeAvailable(teensy_cmake_macros)
include(${teensy_cmake_macros_SOURCE_DIR}/CMakeLists.txt)
set(BUILD_FOR_TEENSY ON)
