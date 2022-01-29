#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico_display.hpp"
//#include "font8_data.hpp"

#include <cstring>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

//#define MODE_SCREEN
#define NUM_WS2812_LEDS 85

#include "button.hpp"
#include "color.hpp"
#include "config.hpp"
#include "debounce.hpp"
#include "pattern.hpp"

#include "WS2812.hpp"



using namespace pimoroni;

int main()
{
    // Ensure in-build LED is off
    constexpr uint DEFAULT_LED_PIN { PICO_DEFAULT_LED_PIN };
    gpio_init(DEFAULT_LED_PIN);
    gpio_set_dir(DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(DEFAULT_LED_PIN, 0);

#ifdef MODE_SCREEN

    // Setup display

    uint16_t display_buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
    PicoDisplay pico_display{ display_buffer };
    pico_display.init();
    pico_display.set_backlight(100);
    pico_display.set_led(0, 0, 0);

#else

    // Setup LEDs

    constexpr uint WS2812_LED_PIN { 9 };
    WS2812 leds(WS2812_LED_PIN, NUM_WS2812_LEDS + 1, pio0, 0);

#endif

#ifdef MODE_SCREEN

    // Button is provided by the screen.
    // The buttons are momentary, so we need to
    // take only the positive edge.
    // The buttons have good physical
    // detents, so not much debounce is needed.

    auto input_pin_fn = [&]()
    {
        return pico_display.is_pressed(pimoroni::PicoDisplay::A)
            | pico_display.is_pressed(pimoroni::PicoDisplay::B)
            | pico_display.is_pressed(pimoroni::PicoDisplay::X)
            | pico_display.is_pressed(pimoroni::PicoDisplay::Y);
    };

    Debounce debounce{ Debounce::EDGE_POSITIVE, 50 };

#else

    // The button is provided by the foot-switch pin.
    // The foot-switch is toggle, so we need to take
    // any edge.
    // Pressing it gently can cause momentary changes,
    // so we need a good amount of debounce.

    constexpr uint FOOTSWITCH_PIN { 10 };
    gpio_init(FOOTSWITCH_PIN);
    gpio_set_dir(FOOTSWITCH_PIN, GPIO_IN);
    gpio_pull_up(FOOTSWITCH_PIN);

    auto input_pin_fn = []()
    {
        return gpio_get(FOOTSWITCH_PIN);        
    };

    Debounce debounce{ Debounce::EDGE_EITHER, 200 };

#endif

    Button button;

    // Setup config

    Config config;
    Config new_config;

    config.load();

    // Setup patterns

    Pattern::ptr off_pattern = Pattern::solid("Off", Color(0, 0, 0));

    std::vector<Pattern::ptr> patterns_by_index;
    patterns_by_index.push_back(Pattern::solid("White", Color(255, 255, 255)));
    patterns_by_index.push_back(Pattern::rainbow());
    patterns_by_index.push_back(Pattern::static_fade("Green Wash", 60, 110));
    patterns_by_index.push_back(Pattern::static_fade("Blue Wash", 100, 230));
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
    uint8_t cur_level = 0;
    uint64_t cur_start_ms = to_ms_since_boot(get_absolute_time());

    if (config.is_on)
    {
        cur_renderer = get_pattern_by_name(config.cur.name)->get_main_render_func();
        cur_level = config.cur.level;
    }

    while (true)
    {
        uint64_t cur_ms = to_ms_since_boot(get_absolute_time());

        // Handle button

        bool pressed = debounce.update(input_pin_fn(), cur_ms);

        bool do_flash = false;
        auto [action, index] = button.update(pressed, cur_ms);

        switch (action)
        {
            case Button::NOTHING:
                break;

            case Button::TOGGLE_OFF:
                new_config = config;
                new_config.is_on = !new_config.is_on;

                if (new_config.is_on)
                {
                    cur_renderer = get_pattern_by_name(new_config.cur.name)->get_preview_render_func();
                    cur_level = new_config.cur.level;
                    cur_start_ms = cur_ms;
                }
                else
                {
                    cur_renderer = off_pattern->get_main_render_func();
                    cur_level = 0;
                    cur_start_ms = cur_ms;
                }

                break;

            case Button::TOGGLE_FAV_PATTERN:
                new_config = config;
                new_config.is_on = true;
                new_config.cur = config.prev;
                new_config.prev = config.cur;

                cur_renderer = get_pattern_by_name(new_config.cur.name)->get_preview_render_func();
                cur_level = new_config.cur.level;
                cur_start_ms = cur_ms;
                break;

            case Button::SELECT_NEW_PATTERN:
                new_config = config;
                new_config.is_on = true;
                new_config.cur = { patterns_by_index[index % patterns_by_index.size()]->get_name(), 255 };
                new_config.prev = config.cur;

                if (new_config.prev.name == new_config.cur.name)
                {
                    new_config.prev = config.prev;
                }

                cur_renderer = get_pattern_by_name(new_config.cur.name)->get_preview_render_func();
                cur_level = new_config.cur.level;
                cur_start_ms = cur_ms;
                break;

            case Button::SELECT_NEW_LEVEL:
                new_config.cur.level = 255 - ((index % 4) * 64);

                cur_renderer = get_pattern_by_name(new_config.cur.name)->get_preview_render_func();
                cur_level = new_config.cur.level;
                cur_start_ms = cur_ms;
                break;

            case Button::SAVE_CONFIG:
                config = new_config;
                config.save();

                if (config.is_on)
                {
                    cur_renderer = get_pattern_by_name(config.cur.name)->get_main_render_func();
                    cur_level = config.cur.level;
                    cur_start_ms = cur_ms;
                }
                else
                {
                    cur_renderer = off_pattern->get_main_render_func();
                    cur_level = 0;
                    cur_start_ms = cur_ms;
                }

                do_flash = true;
                break;

            case Button::FLASH:
                do_flash = true;
                break;
        }

        // Render to buffer

        if (do_flash)
        {
            off_pattern->get_main_render_func()(color_buffer, cur_ms - cur_start_ms);
        }
        else
        {
            cur_renderer(color_buffer, cur_ms - cur_start_ms);
        }

        // Display

        for (uint16_t x = 0; x < Pattern::NUM_LEDS; ++x)
        {
            Color c = color_buffer[x].scale(cur_level);

#ifdef MODE_SCREEN
            pico_display.set_pen(c.red, c.green, c.blue);
            pico_display.line({x, 0}, {x, PicoDisplay::HEIGHT - 1});
#else
            leds.setPixelColor(x + 1, c.red, c.green, c.blue);
#endif
        }

#ifdef MODE_SCREEN
        pico_display.update();
#else
        leds.setPixelColor(0, 0);
        leds.show();
        sleep_ms(20);
#endif        
    }
}
