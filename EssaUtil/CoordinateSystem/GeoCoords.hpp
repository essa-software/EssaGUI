#pragma once

#include "Point.hpp"
#include <fmt/format.h>
#include <EssaUtil/Angle.hpp>

namespace Util {

// GeoCoords is a representation of geographic coordinates, i.e latitude (North/South) and longitude (West/East).
//
// The coordinate system used:
// - Angles start
class GeoCoords {
public:
    GeoCoords(Angle lat, Angle lon)
        : m_latitude(lat)
        , m_longitude(lon) { }

    // Normalize angles to be:
    // - lat ∈ <-π/2; π/2>
    // - lon ∈ <-π; π>
    GeoCoords normalized() const;

    Angle latitude() const { return m_latitude; }
    Angle longitude() const { return m_longitude; }

    template<class T> Detail::Point<3, T> to_cartesian(float radius) const {
        // https://en.wikipedia.org/wiki/Spherical_coordinate_system
        auto norm = normalized();
        auto inclination = norm.latitude().rad() + M_PI / 2;
        auto azimuth = norm.longitude().rad();
        return {
            static_cast<T>(radius * std::sin(inclination) * std::cos(azimuth)),
            static_cast<T>(radius * std::cos(inclination)),
            static_cast<T>(radius * std::sin(inclination) * std::sin(azimuth)),
        };
    }

    bool is_approximately_equal(GeoCoords const& other, float epsilon = 10e-6) const;

private:
    Angle m_latitude;
    Angle m_longitude;
};

}

template<> struct fmt::formatter<Util::GeoCoords> : public fmt::formatter<float> {
    template<typename FormatContext> constexpr auto format(Util::GeoCoords const& coords, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "GeoCoords[");
        ctx.advance_to(fmt::formatter<float>::format(coords.latitude().deg(), ctx));
        fmt::format_to(ctx.out(), ",");
        ctx.advance_to(fmt::formatter<float>::format(coords.longitude().deg(), ctx));
        fmt::format_to(ctx.out(), "]");
        return ctx.out();
    }
};
