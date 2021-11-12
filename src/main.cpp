/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "pico_display.hpp"

using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];

class Button
{
public:
    explicit Button(PicoDisplay &display)
        : m_display(display)
        , m_ticks_off(0)
    {
    }

    bool is_clicked()
    {
        bool cur_pressed = m_display.is_pressed(PicoDisplay::A)
            | m_display.is_pressed(PicoDisplay::B)
            | m_display.is_pressed(PicoDisplay::X)
            | m_display.is_pressed(PicoDisplay::Y);

        bool clicked = false;

        if (cur_pressed)
        {
            if (m_ticks_off > 3)
            {
                clicked = true;
            }
            m_ticks_off = 0;
        }
        else // not on
        {
            if (m_ticks_off < 255)
            {
                m_ticks_off += 1;
            }
        }

        return clicked;
    }
private:
    PicoDisplay& m_display;
    uint8_t m_ticks_off;
};

class Lamp
{
public:
    explicit Lamp(PicoDisplay &display)
        : m_display(display)
        , m_is_on(false)
        , m_on_count(0)
    {
    }

    void toggle()
    {
        m_is_on = !m_is_on;
    }

    void update()
    {
        if (m_is_on)
        {
            m_on_count += 24;

            if (m_on_count > (PicoDisplay::WIDTH - 1))
            {
                m_on_count = PicoDisplay::WIDTH - 1;
            }
        }
        else // off
        {
            if (m_on_count >= 24)
            {
                 m_on_count -= 24;
            }
            else
            {
                m_on_count = 0;
            }
        }

        m_display.set_pen(255, 255, 255);

        for (uint16_t i = 0; i < m_on_count; ++i)
        {
            uint16_t x = PicoDisplay::WIDTH - 1 - i;
            m_display.line({x, 0}, {x, PicoDisplay::HEIGHT - 1});
        }

        m_display.set_pen(0, 0, 0);

        for (uint16_t i = m_on_count; i < PicoDisplay::WIDTH; ++i)
        {
            uint16_t x = PicoDisplay::WIDTH - 1 - i;
            m_display.line({x, 0}, {x, PicoDisplay::HEIGHT - 1});
        }

        m_display.update();
    }

private:
    PicoDisplay& m_display;
    bool m_is_on;
    uint16_t m_on_count;
};

int main()
{
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else

    // Ensure LED is off
    constexpr uint LED_PIN { PICO_DEFAULT_LED_PIN };
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Setup display and button

    PicoDisplay pico_display{ buffer };
    pico_display.init();
    pico_display.set_backlight(100);
    pico_display.set_led(0, 0, 0);

    Button button{ pico_display };
    Lamp lamp{ pico_display };

    while (true) {

        if (button.is_clicked())
        {
            lamp.toggle();
        }

        lamp.update();

        //sleep_ms(16);
    }
#endif
}
