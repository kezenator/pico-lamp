#ifndef __PICO_LAMP__PATTERN_H__
#define __PICO_LAMP__PATTERN_H__

class Pattern
{
public:

#ifdef MODE_SCREEN
    static constexpr size_t NUM_LEDS = pimoroni::PicoDisplay::WIDTH;
#else
    static constexpr size_t NUM_LEDS = NUM_WS2812_LEDS;
#endif

    using RenderFunc = std::function<void(std::array<Color, NUM_LEDS> &buffer, uint64_t animate_ms)>;
    using ptr = std::shared_ptr<Pattern>;

    virtual ~Pattern()
    {
    }

    virtual const std::string &get_name() = 0;
    virtual RenderFunc get_preview_render_func() = 0;
    virtual RenderFunc get_main_render_func() = 0;

    static Pattern::ptr solid(std::string&& name, Color color)
    {
        class SolidPattern : public Pattern
        {
        public:
            ~SolidPattern()
            {
            }

            SolidPattern(std::string&& name, Color color)
                : m_name(std::move(name))
                , m_color(color)
            {
            }

            const std::string &get_name() override
            {
                return m_name;
            }

            RenderFunc get_preview_render_func() override
            {
                return [=](std::array<Color, NUM_LEDS> &buffer, uint64_t /* animate_ms */)
                {
                    for (size_t i = 0; i < NUM_LEDS; ++i)
                    {
                        buffer[i] = m_color;
                    }
                };
            }

            RenderFunc get_main_render_func() override
            {
                return get_preview_render_func();
            }

        private:
            std::string m_name;
            Color m_color;
        };

        return std::make_shared<SolidPattern>(std::move(name), color);
    }

    static Pattern::ptr rainbow()
    {
        class RainbowPattern : public Pattern
        {
        public:
            ~RainbowPattern()
            {
            }

            RainbowPattern()
                : m_name("Rainbow")
            {
            }

            const std::string &get_name() override
            {
                return m_name;
            }

            RenderFunc get_preview_render_func() override
            {
                return [=](std::array<Color, NUM_LEDS> &buffer, uint64_t /* animate_ms */)
                {
                    for (size_t i = 0; i < NUM_LEDS; ++i)
                    {
                        buffer[i] = Color::from_hsv(
                            i * 255 / NUM_LEDS,
                            255,
                            255);
                    }
                };
            }

            RenderFunc get_main_render_func() override
            {
                return [=](std::array<Color, NUM_LEDS> &buffer, uint64_t animate_ms)
                {
                    for (size_t i = 0; i < NUM_LEDS; ++i)
                    {
                        buffer[i] = Color::from_hsv(
                            (animate_ms % 15000) * 255 / 15000
                                + i * 40 / NUM_LEDS,
                            255,
                            255);
                    }
                };
            }

        private:
            std::string m_name;
        };

        return std::make_shared<RainbowPattern>();
    }

    static Pattern::ptr static_fade(std::string&& name, uint8_t hue_1, uint8_t hue_2)
    {
        class StaticFadePattern : public Pattern
        {
        public:
            ~StaticFadePattern()
            {
            }

            StaticFadePattern(std::string&& name, uint8_t hue_1, uint8_t hue_2)
                : m_name(std::move(name))
                , m_hue_1(hue_1)
                , m_hue_2(hue_2)
            {
            }

            const std::string &get_name() override
            {
                return m_name;
            }

            RenderFunc get_preview_render_func() override
            {
                return [=](std::array<Color, NUM_LEDS> &buffer, uint64_t /* animate_ms */)
                {
                    for (size_t i = 0; i < NUM_LEDS; ++i)
                    {
                        buffer[i] = Color::from_hsv(
                            m_hue_1 + i * (m_hue_2 - m_hue_1) / NUM_LEDS,
                            255,
                            255);
                    }
                };
            }

            RenderFunc get_main_render_func() override
            {
                return get_preview_render_func();
            }

        private:
            std::string m_name;
            uint8_t m_hue_1;
            uint8_t m_hue_2;
        };

        return std::make_shared<StaticFadePattern>(std::move(name), hue_1, hue_2);
    }
};

#endif // __PICO_LAMP__PATTERN_H__