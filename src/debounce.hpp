#ifndef __PICO_LAMP__DEBOUNCE_H__
#define __PICO_LAMP__DEBOUNCE_H__

class Debounce
{
public:
    enum E_EDGE
    {
        EDGE_POSITIVE,
        EDGE_EITHER,
    };

    Debounce(E_EDGE edge, uint64_t debounce_ms)
        : m_edge(edge)
        , m_debounce_ms(debounce_ms)
        , m_cur_value(false)
        , m_changing(false)
        , m_change_start_time(0)
    {
    }

    ~Debounce() = default;

    bool update(bool pressed, uint64_t cur_ms)
    {
        if (!m_changing
            && (pressed != m_cur_value))
        {
            // We've first started changing
            m_changing = true;
            m_change_start_time = cur_ms;
        }

        if (m_changing)
        {
            uint64_t expired = cur_ms - m_change_start_time;

            if (expired >= m_debounce_ms)
            {
                m_changing = false;

                if (pressed != m_cur_value)
                {
                    // It's still changed after the debounce time
                    m_cur_value = pressed;

                    if ((pressed && (m_edge == EDGE_POSITIVE))
                        || (m_edge == EDGE_EITHER))
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

private:
    const E_EDGE m_edge;
    const uint64_t m_debounce_ms;
    bool m_cur_value;
    bool m_changing;
    uint64_t m_change_start_time;
};

#endif // __PICO_LAMP__DEBOUNCE_H__