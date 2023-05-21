#pragma once

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Error.hpp>
#include <EssaUtil/UString.hpp>
#include <fstream>
#include <map>

namespace Util {

class ConfigFile {
public:
    static Util::OsErrorOr<ConfigFile> open_ini(std::string const& path);

    // The path is relative to system-specific path:
    // - $HOME/.config/$path on linux
    // - windows is not supported
    static Util::OsErrorOr<ConfigFile> open_user(std::string const& path);

    std::optional<std::string> get(std::string key) const;
    std::optional<Util::Color> get_color(std::string key) const;
    std::optional<uint32_t> get_u32(std::string key) const;
    std::optional<float> get_float(std::string key) const;

private:
    explicit ConfigFile(std::ifstream);

    std::optional<Util::Color> parse_color(std::string const& value) const;
    std::pair<std::string, std::string> parse_line(std::string const& line) const;
    void parse(std::ifstream);

    std::map<std::string, std::string> m_values;
};

}
