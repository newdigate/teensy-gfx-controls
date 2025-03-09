# teensy graphics controls 
[![lib-linux](https://github.com/newdigate/teensy-gfx-controls/actions/workflows/lib_linux.yml/badge.svg)](https://github.com/newdigate/teensy-gfx-controls/actions/workflows/lib_linux.yml)
[![lib_teensy](https://github.com/newdigate/teensy-gfx-controls/actions/workflows/lib_teensy.yml/badge.svg)](https://github.com/newdigate/teensy-gfx-controls/actions/workflows/lib_teensy.yml)

* A portable user-interface library for managing scene controllers, menus, windows and display components.
* This code is built for Arduino, but can be compiled and run on linux and mac osx.
* Optimised for low-memory, and more frequent screen refreshes.  
* targetting 16-bit tft displays 
* **(Work in progress)**

## examples
* [scene controller and menu example](examples/scene_controller/scene_menus.cpp)

  ![scene menus](docs/scene_menus.gif)


## code
* using [newdigate/teensy-gfx](https://github.com/newdigate/teensy-gfx), which is based on
  * [adafruit gfx library](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview) 
  * [PaulStoffregen/ST7735_t3](https://github.com/PaulStoffregen/ST7735_t3)


## cheatsheet
* build & install for linux
``` shell
> mkdir cmake-build-debug
> cd cmake-build-debug
> cmake .. -DCMAKE_BUILD_TYPE=Debug
> cmake --build .
> cmake --install .
```

* build for teensy
```shell
> mkdir cmake-build-teensy-debug
> cd cmake-build-teensy-debug
> cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:FILEPATH="../cmake/toolchains/teensy41.cmake"
> cmake --build .
```
