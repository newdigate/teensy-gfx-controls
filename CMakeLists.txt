cmake_minimum_required(VERSION 3.5)
project(teensy-gfx-controls)
IF (NOT BUILD_FOR_TEENSY)
    set(BUILD_FOR_LINUX 1)
    add_definitions(-DBUILD_FOR_LINUX)
    include(cmake_declare_and_fetch.cmake.in)
    DeclareAndFetch(teensy_x86_stubs            https://github.com/newdigate/teensy-x86-stubs.git main src)
    DeclareAndFetch(teensy_gfx                  https://github.com/newdigate/teensy-gfx.git main src)
    DeclareAndFetch(teensy_x86_encoder_stubs    https://github.com/newdigate/teensy-x86-encoder-stubs.git main src)
    DeclareAndFetch(teensy_x86_bounce2_stubs    https://github.com/newdigate/teensy-x86-bounce2-stubs.git main src)
    DeclareAndFetch(teensy_x86_midi_stubs       https://github.com/newdigate/teensy-x86-midi-stubs main src)

ELSE()
    include(FetchContent)
    FetchContent_Declare(teensy_cmake_macros
            GIT_REPOSITORY https://github.com/newdigate/teensy-cmake-macros
            GIT_TAG        main
    )
    FetchContent_MakeAvailable(teensy_cmake_macros)
    include(${teensy_cmake_macros_SOURCE_DIR}/CMakeLists.include.txt)

    import_arduino_library(cores ${teensy_cores_SOURCE_DIR}/teensy4 avr util)

    import_arduino_library_git(SPI https://github.com/PaulStoffregen/SPI.git master "")
    import_arduino_library_git(Wire https://github.com/PaulStoffregen/Wire master "" utility)
    import_arduino_library_git(adafruit_busio https://github.com/adafruit/Adafruit_BusIO master "")
    import_arduino_library_git(adafruit_gfx https://github.com/adafruit/Adafruit-GFX-Library.git master "")
    import_arduino_library_git(adafruit_st77xx https://github.com/adafruit/Adafruit-ST7735-Library.git master "")
    import_arduino_library_git(SdFat https://github.com/PaulStoffregen/SdFat.git master "src" common DigitalIO ExFatLib FatLib FsLib iostream SdCard SpiDriver)
    import_arduino_library_git(SD https://github.com/PaulStoffregen/SD.git Juse_Use_SdFat src)
    import_arduino_library_git(Encoder https://github.com/PaulStoffregen/Encoder.git master "")
    import_arduino_library_git(Bounce2 https://github.com/PaulStoffregen/Bounce2.git master src)
    import_arduino_library_git(SerialFlash https://github.com/PaulStoffregen/SerialFlash.git master "" util)
    import_arduino_library_git(arm_math https://github.com/PaulStoffregen/arm_math.git master src)
    import_arduino_library_git(TeensyGFX https://github.com/newdigate/teensy-gfx.git main src)
ENDIF()

add_subdirectory(src)

if (BUILD_EXAMPLE)
    if (DEFINED BUILD_FOR_LINUX)
        DeclareAndFetch(teensy_x86_sd_stubs             https://github.com/newdigate/teensy-x86-sd-stubs main src)
        DeclareAndFetch(teensy_audio_x86_stubs          https://github.com/newdigate/teensy-audio-x86-stubs main "src;extras/soundio/src")
        DeclareAndFetch(teensy_st7735_linux_stubs       https://github.com/newdigate/teensy-st7735-linux-stubs main "src;extras/opengl/src")

        DeclareAndFetch(rtmidi                          https://github.com/thestk/rtmidi.git master "/")

        DeclareAndFetchSubLibrary(teensy_x86_midi_stubs     extras/rtmidi/src)
        DeclareAndFetch(teensy_ili9341_controls         https://github.com/newdigate/teensy_ili9341_controls master src)
        DeclareAndFetch(arduino_tftpianoview            https://github.com/newdigate/arduino-tftpianoview.git feature/generic src)

        add_subdirectory(examples/basic)
        #add_subdirectory(examples/demo)
        add_subdirectory(examples/view_example)
        add_subdirectory(examples/menu)
        add_subdirectory(examples/scene_controller)
    endif ()
endif()