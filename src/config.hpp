#ifndef __PICO_LAMP__CONFIG_H__
#define __PICO_LAMP__CONFIG_H__

class Config
{
public:

    struct Selection
    {
        std::string name;
        uint8_t level;

        Selection& operator =(const Selection&) = default;
        Selection& operator =(Selection&&) = default;
    };

    bool is_on;
    Selection cur;
    Selection prev;

    Config()
        : is_on(false)
        , cur{ "White", 255, }
        , prev{ "Red", 255 }
    {
    }

    Config& operator =(const Config&) = default;
    Config& operator =(Config&&) = default;

    void save() const
    {
        std::vector<uint8_t> data;
        data.resize(FLASH_PAGE_SIZE);

        std::string str = to_string();
        std::memcpy(data.data(), str.data(), str.size());

        uint32_t saved = save_and_disable_interrupts();
        flash_range_erase(TARGET_ADDRESS - XIP_BASE, FLASH_SECTOR_SIZE);
        restore_interrupts(saved);

        saved = save_and_disable_interrupts();
        flash_range_program(TARGET_ADDRESS - XIP_BASE, data.data(), FLASH_PAGE_SIZE);
        restore_interrupts(saved);
    }

    bool load()
    {
        bool loaded{ false };

        char *ptr = reinterpret_cast<char *>(TARGET_ADDRESS);

        if (std::memchr(ptr, '\0', FLASH_SECTOR_SIZE))
        {
            std::vector<std::string> lines;

            std::string str{ ptr };
            std::size_t newline_pos;
            while ((newline_pos = str.find('\n')) != std::string::npos)
            {
                lines.push_back(str.substr(0, newline_pos));
                str = str.substr(newline_pos + 1);
            }
            lines.push_back(str);

            if ((lines.size() == 7)
                && (lines[0] == "2021-12-20"))
            {
                is_on = lines[1] == "on";
                cur.name = lines[2];
                cur.level = std::atoi(lines[3].c_str());
                prev.name = lines[4];
                prev.level = std::atoi(lines[5].c_str());

                loaded = true;
            }
        }

        if (!loaded)
        {
            *this = Config();
        }

        return loaded;
    }

private:

    static constexpr uintptr_t TARGET_ADDRESS = XIP_BASE + (2 * 1024 * 1024) - FLASH_SECTOR_SIZE;

    std::string to_string() const
    {
        std::stringstream stream;

        stream << "2021-12-20" << std::endl;
        stream << (is_on ? "on" : "off") << std::endl;
        stream << cur.name << std::endl;
        stream << uint32_t(cur.level) << std::endl;
        stream << prev.name << std::endl;
        stream << uint32_t(prev.level) << std::endl;

        return stream.str();
    }
};

#endif // __PICO_LAMP__CONFIG_H__
