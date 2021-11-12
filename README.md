# pico-lamp
Raspberry Pi Pico code, in C++17, for a floor lamp project I'm working on for Christmas 2021 presents.

## Third-party Code

1. Files imported from other projects:
   1. `src/pico_sdk_import.cmake' from (https://github.com/raspberrypi/pico-examples/blob/master/pico_sdk_import.cmake)
   2. `src/pimoroni_pico_import.cmake` from (https://github.com/pimoroni/pimoroni-pico/blob/main/pimoroni_pico_import.cmake)
2. Libraries it depends on:
   1. [Pico SDK](https://github.com/raspberrypi/pico-sdk).
   2. [Pimoroni Pico libraries](https://github.com/pimoroni/pimoroni-pico)

## Hardware Platform

1. [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/)
2. Development hardware: [Pico Display Pack](https://shop.pimoroni.com/products/pico-display-pack)
3. Real hardware: TBD - haven't built it yet!

## Building the Software

It should build in the same manner as the
[Pico Examples](https://github.com/raspberrypi/pico-examples)
that ship with the
[Pico SDK](https://github.com/raspberrypi/pico-sdk).

I'm building in Windows WSL using the instructions
at https://paulbupejr.com/raspberry-pi-pico-windows-development/.

Currently, WSL Ubuntu provides only GCC 9, so I'm working
in C++17 - which is a shame because I'd like a small project
to try C++20 modules.

In WSL Ubuntu, it should (as at November 2021...) be as easy as:
1. Install the ARM compiler:
   1. `sudo apt update`
   2. `sudo apt install cmake gcc-arm-none-eabi build-essential`
2. Install the required libraries:
   1. `cd ~`
   2. `git clone https://github.com/raspberrypi/pico-sdk.git`
   3. `cd pico-sdk`
   4. `git submodule update --init`
   5. `export PICO_SDK_PATH=~/pico-sdk`
   6. `cd ~`
   7. `git clone https://github.com/pimoroni/pimoroni-pico.git`
   8. `cd pimoroni-pico`
   9. `git submodule update --init`
   10. `export PIMORONI_PICO_PATH=~/pimoroni-pico`
3. Download and build the this pico-lamp project:
   1. `cd ~`
   2. `git clone https://github.com/kezenator/pico-lamp`
   3. `cd pico-lamp`
   4. `cd src`
   5. `cmake .`
   6. `make` (or `make -j8` to speed it up a bit, if you have 8 threads...)
3. Install the software on your Pico board:
   1. Hold down the `BOOTSEL` button and plug the Pico board into your PC.
   2. See that it appears as a USB removable drive.
   3. Copy the `src/pico_lamp.uf2` file to the USB.
   4. NOTE: On WSL, following the instructions above, this can be found at e.g. `\\wsl$\Ubuntu\home\kezenator\pico-lamp\src` in Windows Explorer.
