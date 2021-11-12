# pico-lamp
Raspberry Pi Pico code, in C++17, for a floor lamp project I'm working on for Christmas 2021 presents.

## Third-party Code

1. The file `src/pico_sdk_import.cmake' is copied from the [Pico Examples](https://github.com/raspberrypi/pico-examples/blob/master/pico_sdk_import.cmake).
2. It depends on the [Pico SDK](https://github.com/raspberrypi/pico-sdk).

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

Once all is setup, it's as easy as:
1. `export PICO_SDK_PATH=...`
2. `cd src`
3. `cmake`
4. `make -j8`
5. Hold down the `BOOTSEL` button on your Pico.
6. Copy the `pico_lamp.uf2` file to the USB drive that appears.

Notes:
1. You need to set the `PICO_SDK_PATH` environment variable.
