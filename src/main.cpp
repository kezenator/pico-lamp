/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "pico_display.hpp"

using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];

int main() {

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else

    PicoDisplay pico_display(buffer);
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    pico_display.init();

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);

        if (pico_display.is_pressed(pico_display.A)) {
            pico_display.set_backlight(100);
        } else {
            pico_display.set_backlight(50);
        }

        if (pico_display.is_pressed(pico_display.B)) {
            pico_display.set_led(255, 255, 255);
        } else {
            pico_display.set_led(128, 0, 128);
        }

        pico_display.set_pen(120, 40, 60);
        pico_display.clear();

        if (pico_display.is_pressed(pico_display.X)) {
            pico_display.set_pen(255, 255, 255);
            pico_display.line(Point(50, 50), Point(120, 80));
            pico_display.line(Point(20, 20), Point(120, 20));
            pico_display.line(Point(20, 20), Point(20, 120));
        }

        pico_display.update();
    }
#endif
}
