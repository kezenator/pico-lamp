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
        SELECT_NEW_LEVEL,
        SAVE_CONFIG,
        FLASH,
    };

    Button()
        : m_state(IDLE)
        , m_last_press_time(0)
        , m_press_count(0)
    {
    }

    ~Button() = default;

    std::tuple<Action, uint64_t> update(bool pressed, uint64_t cur_ms)
    {
        Action result_action = NOTHING;
        uint64_t result_count = 0;

        if (pressed)
        {
#ifdef MODE_SCREEN
            // The Pico Display buttons are momentary -
            // so process whenever it goes high
            bool process = pressed;
#else
            // The foot-switch is a toggle, so process
            // any time it changes
            bool process = true;
#endif

            if (process)
            {
                m_last_press_time = cur_ms;

                switch (m_state)
                {
                    case IDLE:
                        m_state = SELECTING_PATTERN;
                        m_press_count = 1;
                        result_action = TOGGLE_OFF;
                        break;

                    case SELECTING_PATTERN:
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
                        break;

                    case SELECTING_LEVEL:
                        m_press_count += 1;
                        
                        result_action = SELECT_NEW_LEVEL;
                        result_count = m_press_count;
                        break;
                }
            }
        }
        else if (m_state != IDLE)
        {
            uint64_t delay = cur_ms - m_last_press_time;

            if (delay > 1500)
            {
                if ((m_state == SELECTING_LEVEL)
                    || (m_press_count <= 2))
                {
                    m_state = IDLE;
                    m_press_count = 0;
                    m_last_press_time = cur_ms;

                    result_action = SAVE_CONFIG;
                }
                else // selecting new pattern
                {
                    m_state = SELECTING_LEVEL;
                    m_press_count = 0;
                    m_last_press_time = cur_ms;

                    result_action = FLASH;
                }
            }
        }

        return std::tie(result_action, result_count);
    }

private:
    enum State
    {
        IDLE,
        SELECTING_PATTERN,
        SELECTING_LEVEL,
    };

    State m_state;
    uint64_t m_last_press_time;
    uint64_t m_press_count;
};

#endif // __PICO_LAMP__BUTTON_H__