#pragma once

#include "UString.hpp"

namespace Util {

struct Unit {
    UString string;
    double multiplier;
};

enum class Quantity {
    Length,
    Mass,
    Velocity,
    Time,
    FileSize,

    None,
};

struct Options {
    // Display precision or 0 for best fit.
    int precision = 2;

    // Precision used for displaying values bigger than 10^7 (in the [ × 10^n] form)
    int scientific_precision = 4;
};

// All strings here are UTF-8 strings.
UString to_exponent_string(double value, Options options);

struct UnitValue {
    UString value {};
    UString unit {};

    UString to_string() const { return value + " " + unit; }
};

// Display value in the form "value[ × 10^n] <unit>", using predefined units.
UnitValue unit_display(double value, Quantity, Options = {});

// Display value in the form "value[ × 10^n] <unit>", using a user-given unit.
UnitValue unit_display(double value, std::vector<Unit>, Options = {});

}
