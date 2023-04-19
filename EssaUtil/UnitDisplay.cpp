#include "UnitDisplay.hpp"

#include "Constants.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

namespace Util {

// FIXME: This could be done at compile time.
// NOTE: Keep the list sorted by sizes ascending, the algorithm relies on this!
static const std::map<Quantity, std::vector<Unit>> s_units {
    { Quantity::Length,
        {
            { "m", 1 },
            { "km", 1000 },
            { "AU", Constants::AU },
            { "ly", Constants::Year* Constants::LightSpeed },
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
            { "c", Constants::LightSpeed },
        } },
    { Quantity::Time,
        {
            { "s", 1 },
            { "m", 60 },
            { "h", 3600 },
            { "days", 3600 * 24 },
            { "years", Constants::Year },
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

UString to_exponent_string(double value, Options options) {
    std::ostringstream oss;
    if (value < 10'000'000) {
        int precision = std::min<int>(fmt::format("{}", (value - std::trunc(value))).size() - 1, options.precision);
        UString string { fmt::format("{:.{}f}", value, precision) };
        if (string.find(".").has_value()) {
            string = string.substring(0, std::min<size_t>(string.size(), options.precision + 2));
            if (string.find(".") == string.size() - 1) {
                return string.substring(0, string.size() - 1);
            }
            return string;
        }
        else {
            return string;
        }
    }
    int exponent = (double)std::log10(value);
    double mantissa = value / std::pow(10, exponent);
    return UString { fmt::format("{:.{}f} \u00D7 10^{}", mantissa, options.scientific_precision, exponent) }; // × (MULTIPLICATION SIGN)
};

UnitValue unit_display(double value, Quantity quantity, Options options) {
    if (quantity == Quantity::None)
        return UnitValue { .value = to_exponent_string(value, options) };
    auto const& units = s_units.find(quantity)->second;
    return unit_display(value, units, options);
}

UnitValue unit_display(double value, std::vector<Unit> units, Options options) {
    for (auto it = units.rbegin(); it != units.rend(); it++) {
        auto const& unit = *it;
        if (value >= unit.multiplier)
            return UnitValue { .value = to_exponent_string(value / unit.multiplier, options), .unit = unit.string };
    }
    return UnitValue { .value = to_exponent_string(value, options), .unit = units[0].string };
}

}
