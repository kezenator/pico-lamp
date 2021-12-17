#ifndef __PICO_LAMP__CONFIG_H__
#define __PICO_LAMP__CONFIG_H__

class Config
{
public:

    bool is_on;
    std::string cur_pattern_name;
    std::string prev_pattern_name;

    Config()
        : is_on(false)
        , cur_pattern_name("White")
        , prev_pattern_name("Red")
    {
    }

    Config& operator =(const Config&) = default;
    Config& operator =(Config&&) = default;
};

#endif // __PICO_LAMP__CONFIG_H__
