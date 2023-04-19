#pragma once

#include "Constants.hpp"

#include <cmath>
#include <ostream>
#include <string>

class Distance {
public:
    enum Unit {
        Meter,
        Kilometer,
        Au
    };

    constexpr Distance() = default;

    constexpr Distance(float v, Unit unit)
        : m_unit(unit)
        , m_value(v) {
    }

    constexpr Unit unit() const { return m_unit; }
    constexpr float value() const { return m_value; }

    constexpr Distance operator-() const { return { -m_value, m_unit }; }
    constexpr bool operator==(Distance const& other) const { return m_unit == other.m_unit && m_value == other.m_value; }

    friend std::ostream& operator<<(std::ostream& out, Distance dist) {
        if (dist.m_unit == Meter)
            return out << dist.value() << " m";
        else if (dist.m_unit == Kilometer)
            return out << dist.value() / 1000 << " km";
        else
            return out << dist.value() / Util::Constants::AU << " AU";
    }

private:
    Unit m_unit = Meter;
    float m_value = 0;
};

constexpr Distance operator""_m(long double v) {
    return Distance(v, Distance::Meter);
}

constexpr Distance operator""_km(long double v) {
    return Distance(v * 1000, Distance::Kilometer);
}

constexpr Distance operator""_AU(long double v) {
    return Distance(v * Util::Constants::AU, Distance::Au);
}
