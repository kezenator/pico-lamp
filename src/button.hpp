#ifndef __PICO_LAMP__BUTTON_H__
#define __PICO_LAMP__BUTTON_H__

class Button
{
public:
    enum Action
    {
        NOTHING,
        TOGGLE_OFF,
        TOGGLE_FAV_PATTERN,
        SELECT_NEW_PATTERN,
        SAVE_CONFIG,
    };

    Button()
        : m_cur_pressed(false)
        , m_last_press_time(0)
        , m_active(false)
        , m_press_count(0)
    {
    }

    ~Button() = default;

    std::tuple<Action, uint64_t> update(bool pressed, uint64_t cur_ms)
    {
        Action result_action = NOTHING;
        uint64_t result_count = 0;

        if (pressed != m_cur_pressed)
        {
            m_cur_pressed = pressed;

            if (pressed)
            {
                m_last_press_time = cur_ms;

                if (!m_active)
                {
                    m_active = true;
                    m_press_count = 1;
                    result_action = TOGGLE_OFF;
                }
                else
                {
                    m_press_count += 1;

                    if (m_press_count == 2)
                    {
                        result_action = TOGGLE_FAV_PATTERN;
                    }
                    else
                    {
                        result_action = SELECT_NEW_PATTERN;
                        result_count = m_press_count - 3;
                    }
                }
            }
        }
        else if (m_active)
        {
            uint64_t delay = cur_ms - m_last_press_time;

            if (delay > 2000)
            {
                m_active = false;
                m_press_count = 0;

                result_action = SAVE_CONFIG;
            }
        }

        return std::tie(result_action, result_count);
    }

private:
    bool m_cur_pressed;
    uint64_t m_last_press_time;
    bool m_active;
    uint64_t m_press_count;
};

#endif // __PICO_LAMP__BUTTON_H__