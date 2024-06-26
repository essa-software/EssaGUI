#pragma once

#include "../Vector.hpp"
#include "Coordinates.hpp"
#include <fmt/format.h>

#ifdef ESSA_COMPILER_GCC
#    pragma GCC optimize("O3")
#endif

namespace Util {

class GeoCoords;

namespace Detail {

template<size_t C, class T>
class Vector;

template<size_t C, class T>
class Point : public Coordinates<C, T, Point> {
public:
    using Super = Coordinates<C, T, Point>;
    using ThisVector = Vector<C, T>;

    // -> Interoperability
    ThisVector to_vector() const;

    template<class... Args>
        requires requires(Args... a) { Super(std::forward<Args>(a)...); }
    Point(Args... a)
        : Super(std::forward<Args>(a)...) { }

    template<class OtherT>
    static Point from_deprecated_vector(DeprecatedVector<C, OtherT> const& c) {
        Point p;
        for (size_t s = 0; s < Super::Components; s++) {
            p.set_component(s, c.components[s]);
        }
        return p;
    }

    DeprecatedVector<C, T> to_deprecated_vector() const {
        DeprecatedVector<C, T> p;
        for (size_t s = 0; s < Super::Components; s++) {
            p.components[s] = this->component(s);
        }
        return p;
    }

    double distance_squared(Point const& b) const {
        double sum = 0;
        // Note: Doesn't add W coordinate
        for (size_t s = 0; s < std::min<size_t>(3, Super::Components); s++) {
            sum += std::pow(this->component(s) - b.component(s), 2);
        }
        return sum;
    }

    double distance(Point const& b) const {
        return std::sqrt(distance_squared(b));
    }

    // Point + Vector
    constexpr Point operator+(ThisVector const& b) const {
        Point ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) + b.component(s));
        }
        return ab;
    }

    constexpr Point& operator+=(ThisVector const& b) {
        return *this = *this + b;
    }

    // Point - Vector
    constexpr Point operator-(ThisVector const& b) const {
        Point ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) - b.component(s));
        }
        return ab;
    }

    constexpr Point& operator-=(ThisVector const& b) {
        return *this = *this - b;
    }

    // -Point
    constexpr Point operator-() const {
        Point ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, -this->component(s));
        }
        return ab;
    }

    // Point - Point
    constexpr ThisVector operator-(Point const& b) const {
        ThisVector ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) - b.component(s));
        }
        return ab;
    }

    constexpr Point operator*(double x) const {
        Point ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) * x);
        }
        return ab;
    }

    constexpr Point& operator*=(double x) {
        return *this = *this * x;
    }

    constexpr Point operator/(double x) const {
        assert(x != 0);
        Point ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) / x);
        }
        return ab;
    }

    constexpr Point& operator/=(double x) {
        return *this = *this / x;
    }

    //// Point2 ////
    template<size_t OtherC, class OtherT>
        requires(Super::Components == 2 && OtherC >= 2)
    constexpr explicit Point(Point<OtherC, OtherT> other)
        : Point { other.x(), other.y() } { }

    constexpr static Point from_main_cross(Orientation orientation, T main, T cross)
        requires(Super::Components == 2)
    {
        if (orientation == Orientation::Vertical)
            return { cross, main };
        return { main, cross };
    }

    constexpr T main(Orientation orientation) const
        requires(Super::Components == 2)
    {
        return orientation == Orientation::Vertical ? this->y() : this->x();
    }

    constexpr T cross(Orientation orientation) const
        requires(Super::Components == 2)
    {
        return orientation == Orientation::Vertical ? this->x() : this->y();
    }

    // Angle is CCW starting from positive X axis.
    constexpr static Point create_polar(Angle angle, double length)
        requires(Super::Components == 2)
    {
        return { std::cos(angle.rad()) * length, std::sin(angle.rad()) * length };
    }

    //// Point3 ////
    template<size_t OtherC, class OtherT>
        requires(Super::Components == 3 && OtherC >= 3)
    constexpr explicit Point(Point<OtherC, OtherT> other)
        : Point { other.x(), other.y(), other.z() } { }

    constexpr static Point create_spheric(Angle lat, Angle lon, double radius)
        requires(C == 3);

    constexpr static Point create_spheric(GeoCoords const& coords, double radius)
        requires(C == 3);

    //// Point4 ////
    template<size_t OtherC, class OtherT>
        requires(Super::Components == 4 && OtherC >= 4)
    constexpr explicit Point(Point<OtherC, OtherT> other)
        : Point { other.x(), other.y(), other.z(), other.w() } { }

    bool operator==(Point const&) const = default;
};

} // Detail

} // Util

template<size_t C, class T>
class fmt::formatter<Util::Detail::Point<C, T>> : public fmt::formatter<T> {
public:
    template<typename FormatContext>
    constexpr auto format(Util::Detail::Point<C, T> const& v, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "(");
        for (size_t s = 0; s < C; s++) {
            ctx.advance_to(fmt::formatter<T>::format(v.component(s), ctx));
            if (s != C - 1)
                fmt::format_to(ctx.out(), ", ");
        }
        return fmt::format_to(ctx.out(), ")");
        return ctx.out();
    }
};
