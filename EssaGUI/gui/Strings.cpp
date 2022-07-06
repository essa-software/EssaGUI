#include "Strings.hpp"

#include <SFML/System.hpp>

namespace GUI {

sf::String utf8_to_sf_string(std::u8string const& str) {
    return sf::String::fromUtf8(str.begin(), str.end()).toAnsiString();
}

}
