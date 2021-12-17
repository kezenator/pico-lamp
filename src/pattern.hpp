#ifndef __PICO_LAMP__PATTERN_H__
#define __PICO_LAMP__PATTERN_H__

class Pattern
{
public:
    static constexpr size_t NUM_LEDS = pimoroni::PicoDisplay::WIDTH;

    using RenderFunc = std::function<void(std::array<Color, NUM_LEDS> &buffer)>;
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
                return [=](std::array<Color, NUM_LEDS> &buffer)
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
};

#endif // __PICO_LAMP__PATTERN_H__