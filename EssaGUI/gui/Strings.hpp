#pragma once

#include <SFML/System.hpp>
#include <string>

namespace GUI {

sf::String utf8_to_sf_string(std::u8string const& str);

}
