#include "UnitDisplay.hpp"

#include "Constants.hpp"

#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <vector>

namespace Util {

struct Unit {
    std::string string;
    double multiplier;
};

// FIXME: This could be done at compile time.
// NOTE: Keep the list sorted by sizes ascending, the algorithm relies on this!
static const std::map<Quantity, std::vector<Unit>> s_units {
    { Quantity::Length,
        {
            { "m", 1 },
            { "km", 1000 },
            { "AU", AU },
            { "ly", Year* LightSpeed },
        } },
    { Quantity::Mass,
        {
            { "kg", 1 },
            { "Tons", 1000 },
        } },
    { Quantity::Velocity,
        {
            { "m/s", 1 },
            { "km/s", 1000 },
            { "c", LightSpeed },
        } },
    { Quantity::Time,
        {
            { "s", 1 },
            { "m", 60 },
            { "h", 3600 },
            { "days", 3600 * 24 },
            { "years", Year },
        } },
    { Quantity::FileSize,
        {
            { "B", 1 },
            { "KiB", 1024 },
            { "MiB", 1024 * 1024 },
            { "GiB", 1024 * 1024 * 1024 },
            { "TiB", 1024LL * 1024 * 1024 * 1024 },
        } }
};

sf::String to_exponent_string(double value) {
    std::ostringstream oss;
    if (value < 10) {
        oss << std::setprecision(2) << value;
        return oss.str();
    }
    if (value < 10'000'000) {
        oss << std::fixed << std::setprecision(2) << value;
        return oss.str();
    }
    int exponent = (double)std::log10(value);
    double mantissa = value / std::pow(10, exponent);
    oss << std::fixed << std::setprecision(4) << mantissa << " \u00D7 10^" << exponent; // × (MULTIPLICATION SIGN)
    auto oss_str = oss.str();
    return sf::String::fromUtf8(oss_str.begin(), oss_str.end());
};

UnitValue unit_display(double value, Quantity quantity) {
    if (quantity == Quantity::None)
        return UnitValue { .value = to_exponent_string(value) };
    auto const& units = s_units.find(quantity)->second;
    for (auto it = units.rbegin(); it != units.rend(); it++) {
        auto const& unit = *it;
        if (value >= unit.multiplier)
            return UnitValue { .value = to_exponent_string(value / unit.multiplier), .unit = unit.string };
    }
    return UnitValue { .value = to_exponent_string(value), .unit = units[0].string };
}

}
