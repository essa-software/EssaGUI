#include "ConfigFile.hpp"

namespace Util {

ConfigFile::ConfigFile(std::ifstream input) {
    parse(std::move(input));
}

std::optional<Util::Color> ConfigFile::parse_color(std::string const& value) const {
    if (value == "White")
        return Util::Colors::White;

    Util::Color color;
    uint8_t color_arr[4] { 0 };
    color_arr[3] = 255;
    size_t i = 0;

    for (const auto& c : value) {
        if (c == ',') {
            i++;

            if (i == 4)
                break;
            continue;
        }

        // TODO: Add overflow check
        if (c >= '0' && c <= '9')
            color_arr[i] = color_arr[i] * 10 + (c - 48);
    }

    i++;
    if (i < 3) {
        std::cout << "Not enough color components, expected at least 3 but got " << i << std::endl;
        return {};
    }

    color.r = color_arr[0];
    color.g = color_arr[1];
    color.b = color_arr[2];
    color.a = color_arr[3];
    return color;
}

std::pair<std::string, std::string> ConfigFile::parse_line(std::string const& line) const {
    std::string key = "";
    std::string value = "";
    bool in_value = false;
    for (const auto c : line) {
        if (c == ' ' || c == '\t')
            continue;
        else if (c == '=') {
            in_value = true;
            continue;
        }
        else if (c == '#')
            break;

        if (in_value)
            value += c;
        else
            key += c;
    }

    return { key, value };
}

void ConfigFile::parse(std::ifstream input) {
    while (!input.eof()) {
        std::string line;
        std::getline(input, line);

        auto val = parse_line(line);

        if (val.first.size() == 0)
            continue;

        m_values.insert(val);
    }
}

Util::OsErrorOr<ConfigFile> ConfigFile::open_ini(std::string const& path) {
    std::ifstream file { path };
    if (file.fail())
        return Util::OsError { .error = errno, .function = "open config file" };

    return ConfigFile { std::move(file) };
}

std::optional<std::string> ConfigFile::get(std::string key) const {
    auto it = m_values.find(key);
    if (it == m_values.end())
        return {};
    return it->second;
}

std::optional<Util::Color> ConfigFile::get_color(std::string key) const {
    auto value = get(key);
    if (!value)
        return {};
    if (auto color = parse_color(*value))
        return color;
    return {};
}

}
