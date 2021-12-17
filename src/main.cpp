#include "pico/stdlib.h"
#include "pico_display.hpp"
//#include "font8_data.hpp"

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "button.hpp"
#include "color.hpp"
#include "config.hpp"
#include "pattern.hpp"

using namespace pimoroni;

int main()
{
    // Ensure LED is off
    constexpr uint LED_PIN { PICO_DEFAULT_LED_PIN };
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Setup display

    uint16_t display_buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
    PicoDisplay pico_display{ display_buffer };
    pico_display.init();
    pico_display.set_backlight(100);
    pico_display.set_led(0, 0, 0);

    // Setup button

    Button button;

    // Setup config

    Config config;
    Config new_config;

    // Setup patterns

    Pattern::ptr off_pattern = Pattern::solid("Off", Color(0, 0, 0));

    std::vector<Pattern::ptr> patterns_by_index;
    patterns_by_index.push_back(Pattern::solid("White", Color(255, 255, 255)));
    patterns_by_index.push_back(Pattern::solid("Red", Color(255, 0, 0)));
    patterns_by_index.push_back(Pattern::solid("Orange", Color(255, 128, 0)));
    patterns_by_index.push_back(Pattern::solid("Yellow", Color(255, 255, 0)));
    patterns_by_index.push_back(Pattern::solid("Green", Color(0, 255, 0)));
    patterns_by_index.push_back(Pattern::solid("Cyan", Color(0, 255, 255)));
    patterns_by_index.push_back(Pattern::solid("Blue", Color(0, 0, 255)));
    patterns_by_index.push_back(Pattern::solid("Violet", Color(128, 0, 255)));
    patterns_by_index.push_back(Pattern::solid("Magenta", Color(255, 0, 255)));

    std::map<std::string, Pattern::ptr> patterns_by_name;
    for (const auto &pattern : patterns_by_index)
    {
        patterns_by_name[pattern->get_name()] = pattern;
    }

    auto get_pattern_by_name = [&](const std::string &name) -> Pattern::ptr
    {
        auto it = patterns_by_name.find(name);
        if (it == patterns_by_name.end())
            return patterns_by_index[0];
        return it->second;
    };

    // Setup renderer

    std::array<Color, Pattern::NUM_LEDS> color_buffer;

    Pattern::RenderFunc cur_renderer = off_pattern->get_main_render_func();

    while (true)
    {
        uint64_t cur_ms = to_ms_since_boot(get_absolute_time());

        // Handle button

        bool pressed = pico_display.is_pressed(pimoroni::PicoDisplay::A)
            | pico_display.is_pressed(pimoroni::PicoDisplay::B)
            | pico_display.is_pressed(pimoroni::PicoDisplay::X)
            | pico_display.is_pressed(pimoroni::PicoDisplay::Y);

        auto [action, index] = button.update(pressed, cur_ms);
        switch (action)
        {
            case Button::NOTHING:
                break;

            case Button::TOGGLE_OFF:
                new_config = config;
                new_config.is_on = !new_config.is_on;

                if (new_config.is_on)
                    cur_renderer = get_pattern_by_name(new_config.cur_pattern_name)->get_main_render_func();
                else
                    cur_renderer = off_pattern->get_main_render_func();

                break;

            case Button::TOGGLE_FAV_PATTERN:
                new_config = config;
                new_config.is_on = true;
                std::swap(new_config.cur_pattern_name, new_config.prev_pattern_name);

                cur_renderer = get_pattern_by_name(new_config.cur_pattern_name)->get_main_render_func();
                break;

            case Button::SELECT_NEW_PATTERN:
                new_config = config;
                new_config.is_on = true;
                new_config.cur_pattern_name = patterns_by_index[index % patterns_by_index.size()]->get_name();
                new_config.prev_pattern_name = config.cur_pattern_name;

                if (new_config.prev_pattern_name == new_config.cur_pattern_name)
                {
                    new_config.prev_pattern_name = config.prev_pattern_name;
                }

                cur_renderer = get_pattern_by_name(new_config.cur_pattern_name)->get_main_render_func();
                break;

            case Button::SAVE_CONFIG:
                config = new_config;
                break;
        }

        // Render to buffer

        cur_renderer(color_buffer);

        // Display

        for (uint16_t x = 0; x < PicoDisplay::WIDTH; ++x)
        {
            pico_display.set_pen(color_buffer[x].red, color_buffer[x].green, color_buffer[x].blue);
            pico_display.line({x, 0}, {x, PicoDisplay::HEIGHT - 1});
        }
        pico_display.update();
    }
}
