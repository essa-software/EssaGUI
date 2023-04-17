#include "ConfigFile.hpp"

#include <sstream>
#include <utility>

namespace Util {

ConfigFile::ConfigFile(std::ifstream input) {
    parse(std::move(input));
}

std::optional<Util::Color> ConfigFile::parse_color(std::string const& value) const {
    if (value == "White")
        return Util::Colors::White;

    Util::Color color;
    color.a = 255;

    if (!value.starts_with("rgb(")) {
        return {};
    }

    std::istringstream iss { value.substr(4) };
    int r;
    int g;
    int b;
    int a = 255;

    // R
    if (!(iss >> r)) {
        std::cerr << "ConfigFile: expected R component" << std::endl;
        return {};
    }
    iss >> std::ws;
    if (iss.get() != ',') {
        std::cerr << "ConfigFile: expected ',' after R component" << std::endl;
        return {};
    }

    // G
    if (!(iss >> g)) {
        std::cerr << "ConfigFile: expected G component" << std::endl;
        return {};
    }
    iss >> std::ws;
    if (iss.get() != ',') {
        std::cerr << "ConfigFile: expected ',' after G component" << std::endl;
        return {};
    }

    // B
    if (!(iss >> b)) {
        std::cerr << "ConfigFile: expected B component" << std::endl;
        return {};
    }

    // optional A
    iss >> std::ws;
    if (iss.peek() == ',') {
        iss.get();
        if (!(iss >> a)) {
            std::cerr << "ConfigFile: expected A component" << std::endl;
            return {};
        }
    }

    iss >> std::ws;
    if (auto c = iss.get(); c != ')') {
        std::cerr << "ConfigFile: expected ')', got '" << c << std::endl;
        return {};
    }

    if (r > 255 || r < 0) {
        std::cerr << "ConfigFile: R component out of range (R=" << r << ", expected 0..255)" << std::endl;
        return {};
    }
    if (g > 255 || g < 0) {
        std::cerr << "ConfigFile: G component out of range (G=" << r << ", expected 0..255)" << std::endl;
        return {};
    }
    if (b > 255 || b < 0) {
        std::cerr << "ConfigFile: B component out of range (B=" << r << ", expected 0..255)" << std::endl;
        return {};
    }
    if (a > 255 || a < 0) {
        std::cerr << "ConfigFile: A component out of range (A=" << r << ", expected 0..255)" << std::endl;
        return {};
    }

    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

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
    auto value = get(std::move(key));
    if (!value)
        return {};
    if (auto color = parse_color(*value))
        return color;
    return {};
}

std::optional<uint32_t> ConfigFile::get_u32(std::string key) const {
    auto value = get(std::move(key));
    if (!value)
        return {};

    try {
        return std::stoul(*value);
    } catch (...) {
        std::cerr << "ConfigFile: could not parse i32" << std::endl;
        return {};
    }
}

std::optional<float> ConfigFile::get_float(std::string key) const {
    auto value = get(std::move(key));
    if (!value)
        return {};

    try {
        return std::stof(*value);
    } catch (...) {
        std::cerr << "ConfigFile: could not parse float" << std::endl;
        return {};
    }
}

}
