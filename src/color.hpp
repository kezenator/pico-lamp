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

    Color scale(uint8_t level) const
    {
        return Color(
            red * level / 255,
            green * level / 255,
            blue * level / 255
        );
    }

    static Color from_hsv(uint8_t hue, uint8_t sat, uint8_t val)
    {
        float h = float(hue) * (360.0f / 256.0f);
        float s = float(sat) / 255.0f;
        float v = float(val) / 255.0f;

        // Taken from:
        // http://www.cs.rit.edu/~ncs/color/t_convert.html
        // on 6th March 2015

        int i;
        float f, p, q, t;
        float r, g, b;

        if (s == 0)
        {
            // achromatic (grey)
            r = g = b = v;
        }
        else
        {
            h /= 60;            // sector 0 to 5
            i = int(h);
            f = h - i;          // fractional part of h
            p = v * (1 - s);
            q = v * (1 - s * f);
            t = v * (1 - s * (1 - f));

            switch (i)
            {
            case 0:
                r = v;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = v;
                b = p;
                break;
            case 2:
                r = p;
                g = v;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = v;
                break;
            case 4:
                r = t;
                g = p;
                b = v;
                break;
            default:		// case 5:
                r = v;
                g = p;
                b = q;
                break;
            }
        }

        return Color(uint8_t((r * 255.0) + 0.5),
                        uint8_t((g * 255.0) + 0.5),
                        uint8_t((b * 255.0) + 0.5));    }
};

#endif // __PICO_LAMP__BUTTON_H__