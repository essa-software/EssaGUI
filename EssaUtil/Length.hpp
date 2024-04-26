#pragma once

#include <EssaUtil/Config.hpp>
#include <ostream>

namespace Util {

class Length {
public:
    enum Unit { Initial, Auto, Px, Percent };

    constexpr Length() = default;

    constexpr Length(Unit unit)
        : m_unit(unit) {
        if (unit == Unit::Auto) {
            m_value = 1;
        }
    }

    constexpr Length(float v, Unit unit)
        : m_value(v)
        , m_unit(unit) { }

    constexpr Unit unit() const {
        return m_unit;
    }
    constexpr float value() const {
        return m_value;
    }

    constexpr bool operator==(Length const& other) const {
        return m_unit == other.m_unit && m_value == other.m_value;
    }

private:
    friend std::ostream& operator<<(std::ostream& out, Length const& l) {
        switch (l.m_unit) {
        case Length::Initial:
            return out << "initial";
        case Length::Auto:
            return out << l.m_value << "fr";
        case Length::Px:
            return out << l.m_value << "px";
        case Length::Percent:
            return out << l.m_value << "%";
        }
        ESSA_UNREACHABLE;
    }

    float m_value = 1;
    Unit m_unit = Auto;
};

}

constexpr Util::Length operator""_px(unsigned long long v) {
    return Util::Length(static_cast<float>(v), Util::Length::Px);
}

constexpr Util::Length operator""_perc(unsigned long long v) {
    return Util::Length(static_cast<float>(v), Util::Length::Percent);
}

// Kept for compatibility, for now.
// FIXME: Remove these.
// FIXME 2: Move this thing to GUI, actually.
constexpr Util::Length operator""_px(long double v) {
    return Util::Length(static_cast<float>(v), Util::Length::Px);
}

constexpr Util::Length operator""_perc(long double v) {
    return Util::Length(static_cast<float>(v), Util::Length::Percent);
}
