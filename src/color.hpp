#ifndef __PICO_LAMP__COLOR_H__
#define __PICO_LAMP__BUTTON_H__

struct Color
{
public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    Color() = default;
    Color(const Color&) = default;
    Color &operator =(const Color&) = default;
    ~Color() = default;

    Color(uint8_t _red, uint8_t _green, uint8_t _blue)
        : red(_red), green(_green), blue(_blue)
    {
    }
};

#endif // __PICO_LAMP__BUTTON_H__