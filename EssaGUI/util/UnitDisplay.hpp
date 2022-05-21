#pragma once

#include <SFML/System.hpp>

namespace Util
{

enum class Quantity
{
    Length,
    Mass,
    Velocity,
    Time,
    FileSize,

    None,
};

sf::String to_exponent_string(double value);

struct UnitValue
{
    sf::String value {};
    sf::String unit {};

    sf::String to_string() const { return value + " " + unit; }
};

UnitValue unit_display(double value, Quantity);

}
