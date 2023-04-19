#pragma once

#include "Angle.hpp"
#include "Orientation.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fmt/core.h>
#include <initializer_list>
#include <ostream>

namespace Util {

namespace Detail {

template<size_t C, class T>
class DeprecatedVector {
public:
    static constexpr size_t Components = C;

    constexpr DeprecatedVector() {
        if constexpr (Components == 4)
            components[3] = 1;
    }

    template<class... T2>
    constexpr DeprecatedVector(T2... packed_c)
        requires(sizeof...(packed_c) == Components || (Components == 4 && sizeof...(packed_c) == 3))
    {
        auto c = std::initializer_list<T> { static_cast<T>(packed_c)... };
        std::copy(std::begin(c), std::end(c), components.begin());
        if constexpr (Components == 4 && sizeof...(packed_c) == 3)
            components[3] = 1;
        assert_components_are_finite();
    }

    template<size_t OtherC, class OtherT, class... MoreT>
        requires(OtherC + sizeof...(MoreT) == Components || (Components == 4 && OtherC + sizeof...(MoreT) == 3))
    constexpr explicit DeprecatedVector(DeprecatedVector<OtherC, OtherT> const& other, MoreT... more) {
        std::copy(other.components.begin(), other.components.end(), components.begin());
        auto c = std::initializer_list<T> { static_cast<T>(more)... };
        std::copy(std::begin(c), std::end(c), components.begin() + OtherC);
        if constexpr (Components == 4 && OtherC + sizeof...(MoreT) == 3)
            components[3] = 1;
        assert_components_are_finite();
    }

    constexpr T& x()
        requires(Components > 0)
    { return this->components[0]; };
    constexpr T& y()
        requires(Components > 1)
    { return this->components[1]; };
    constexpr T& z()
        requires(Components > 2)
    { return this->components[2]; };
    constexpr T& w()
        requires(Components > 3)
    { return this->components[3]; };

    constexpr T const& x() const
        requires(Components > 0)
    { return this->components[0]; };
    constexpr T const& y() const
        requires(Components > 1)
    { return this->components[1]; };
    constexpr T const& z() const
        requires(Components > 2)
    { return this->components[2]; };
    constexpr T const& w() const
        requires(Components > 3)
    { return this->components[3]; };

    bool is_null_vector() const {
        return std::ranges::all_of(components, [](auto n) { return n == 0; });
    }

    bool is_approximately_equal(DeprecatedVector const& other, float epsilon = 10e-6) {
        for (size_t s = 0; s < Components; s++) {
            if (std::abs(other.components[s] - components[s]) > epsilon) {
                return false;
            }
        }
        return true;
    }

    auto length_squared() const {
        double result = 0;
        for (size_t s = 0; s < Components; s++)
            result += components[s] * components[s];
        return result;
    }

    auto length() const {
        return std::sqrt(length_squared());
    }

    double inverted_length() const {
        return 1 / length();
    }

    // Return unit vector of direction the same as original. If original
    // vector is null, return null.
    auto normalized() const {
        if (is_null_vector()) {
            return DeprecatedVector {};
        }
        return *this * inverted_length();
    }

    bool is_normalized() const {
        auto length = length_squared();
        return length == 1 || length == 0;
    }

    double dot(DeprecatedVector const& a) const {
        double result = 0;
        for (size_t s = 0; s < std::min<size_t>(Components, 3); s++)
            result += components[s] * a.components[s];
        return result;
    }

    auto floored() const {
        DeprecatedVector output;
        for (size_t s = 0; s < Components; s++) {
            output.components[s] = std::floor(components[s]);
        }
        output.assert_components_are_finite();
        return output;
    }

    auto ceiled() const {
        DeprecatedVector output;
        for (size_t s = 0; s < Components; s++) {
            output.components[s] = std::ceil(components[s]);
        }
        output.assert_components_are_finite();
        return output;
    }

    auto rounded() const {
        DeprecatedVector output;
        for (size_t s = 0; s < Components; s++) {
            output.components[s] = std::round(components[s]);
        }
        output.assert_components_are_finite();
        return output;
    }

    // Returns `this` resized to `length`, i.e this.normalized()*length.
    // If `this` is a null vector, this function returns null vector.
    constexpr DeprecatedVector with_length(double length) const {
        if (is_null_vector()) {
            return DeprecatedVector {};
        }
        DeprecatedVector normalized = this->normalized();
        DeprecatedVector output;
        for (size_t s = 0; s < Components; s++) {
            output.components[s] = normalized.components[s] * length;
        }
        output.assert_components_are_finite();
        return output;
    }

    constexpr DeprecatedVector operator+(DeprecatedVector const& b) const {
        DeprecatedVector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] + b.components[s];
        ab.assert_components_are_finite();
        return ab;
    }

    constexpr DeprecatedVector& operator+=(DeprecatedVector const& b) {
        return *this = *this + b;
    }

    constexpr DeprecatedVector operator-(DeprecatedVector const& b) const {
        DeprecatedVector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] - b.components[s];
        ab.assert_components_are_finite();
        return ab;
    }

    constexpr DeprecatedVector& operator-=(DeprecatedVector const& b) {
        return *this = *this - b;
    }

    constexpr DeprecatedVector operator*(double x) const {
        DeprecatedVector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] * x;
        ab.assert_components_are_finite();
        return ab;
    }

    constexpr DeprecatedVector& operator*=(double x) {
        return *this = *this * x;
    }

    constexpr DeprecatedVector operator/(double x) const {
        DeprecatedVector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] / x;
        ab.assert_components_are_finite();
        return ab;
    }

    constexpr DeprecatedVector& operator/=(double x) {
        return *this = *this / x;
    }

    constexpr DeprecatedVector operator-() const {
        DeprecatedVector ap;
        for (size_t s = 0; s < Components; s++)
            ap.components[s] = -components[s];
        ap.assert_components_are_finite();
        return ap;
    }

    //// Vector2 ////
    template<size_t OtherC, class OtherT>
        requires(Components == 2 && OtherC >= 2)
    constexpr explicit DeprecatedVector(DeprecatedVector<OtherC, OtherT> other)
        : DeprecatedVector { other.x(), other.y() } {
    }

    constexpr static DeprecatedVector from_main_cross(Orientation orientation, T main, T cross)
        requires(Components == 2)
    {
        if (orientation == Orientation::Vertical)
            return { cross, main };
        return { main, cross };
    }

    // Angle is CCW starting from positive X axis.
    constexpr static DeprecatedVector create_polar(double angle_radians, double length)
        requires(Components == 2)
    {
        return { std::cos(angle_radians) * length, std::sin(angle_radians) * length };
    }

    constexpr double angle() const
        requires(Components == 2)
    {
        return std::atan2(this->y(), this->x());
    }

    constexpr Angle directed_angle_to(DeprecatedVector<2, T> other) const
        requires(Components == 2)
    {
        auto angle1 = angle();
        auto angle2 = other.angle();
        return Angle::radians(angle2 - angle1);
    }

    template<class OtherT = T>
        requires(Components == 2)
    constexpr DeprecatedVector<2, OtherT> rotate(double theta) const {
        double t_cos = std::cos(theta), t_sin = std::sin(theta);
        return { this->x() * t_cos - this->y() * t_sin, this->x() * t_sin + this->y() * t_cos };
    }

    template<class OtherT = T>
        requires(Components == 2)
    constexpr auto perpendicular() const {
        return DeprecatedVector<2, OtherT> { -this->y(), this->x() };
    }

    template<class OtherT>
        requires(Components == 2)
    constexpr auto mirror(DeprecatedVector<2, OtherT> axis) const {
        return *this - (decltype(this->x()))2 * dot(axis.normalized()) * axis;
    }

    constexpr auto main(Orientation orientation) const
        requires(Components == 2)
    {
        if (orientation == Orientation::Vertical)
            return y();
        return x();
    }

    constexpr auto cross(Orientation orientation) const
        requires(Components == 2)
    {
        if (orientation == Orientation::Horizontal)
            return y();
        return x();
    }

    //// Vector3 ////
    template<size_t OtherC, class OtherT>
        requires(Components == 3 && OtherC >= 3)
    constexpr explicit DeprecatedVector(DeprecatedVector<OtherC, OtherT> other)
        : DeprecatedVector { other.x(), other.y(), other.z() } {
    }

    constexpr static DeprecatedVector create_spheric(double lat_radians, double lon_radians, double radius)
        requires(Components == 3)
    {
        return {
            static_cast<T>(radius * std::cos(lat_radians) * std::sin(lon_radians)),
            static_cast<T>(radius * std::sin(lat_radians) * std::sin(lon_radians)),
            static_cast<T>(radius * std::cos(lon_radians)),
        };
    }

    template<class OtherT = T>
        requires(Components == 3)
    constexpr DeprecatedVector<3, OtherT> rotate_x(double theta) const {
        double t_cos = std::cos(theta), t_sin = std::sin(theta);
        return { this->x(), this->y() * t_cos - this->z() * t_sin, this->y() * t_sin + this->z() * t_cos };
    }

    template<class OtherT = T>
        requires(Components == 3)
    constexpr DeprecatedVector<3, OtherT> rotate_y(double theta) const {
        double t_cos = std::cos(theta), t_sin = std::sin(theta);
        return { this->x() * t_cos - this->z() * t_sin, this->y(), this->x() * t_sin + this->z() * t_cos };
    }

    template<class OtherT = T>
        requires(Components == 3)
    constexpr DeprecatedVector<3, OtherT> rotate_z(double theta) const {
        double t_cos = std::cos(theta), t_sin = std::sin(theta);
        return { this->x() * t_cos - this->y() * t_sin, this->x() * t_sin + this->y() * t_cos, this->z() };
    }

    template<class OtherT = T>
        requires(Components == 3)
    constexpr DeprecatedVector<3, OtherT> cross(DeprecatedVector<3, T> const& a) const {
        DeprecatedVector<3, OtherT> result;
        result.x() = this->y() * a.z() - this->z() * a.y();
        result.y() = this->z() * a.x() - this->x() * a.z();
        result.z() = this->x() * a.y() - this->y() * a.x();
        result.assert_components_are_finite();
        return result;
    }

    //// Vector4 ////

    template<size_t OtherC, class OtherT>
        requires(Components == 4 && OtherC >= 4)
    constexpr explicit DeprecatedVector(DeprecatedVector<OtherC, OtherT> other)
        : DeprecatedVector { other.x(), other.y(), other.z(), other.w() } {
    }

    bool operator==(DeprecatedVector const&) const = default;

    friend std::ostream& operator<<(std::ostream& out, DeprecatedVector const& v) {
        return out << fmt::format("{}", v);
    }

    std::array<T, Components> components {};

private:
    void assert_components_are_finite() const {
        for ([[maybe_unused]] auto c : components) {
            assert(std::isfinite(c));
        }
    }
};

}

template<size_t C, class T>
Detail::DeprecatedVector<C, T> operator*(double fac, Detail::DeprecatedVector<C, T> const& vec) {
    return vec * fac;
}

template<class T>
using Vector2 = Detail::DeprecatedVector<2, T>;
using Vector2i = Vector2<int>;
using Vector2u = Vector2<unsigned>;
using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

template<class T>
using Vector3 = Detail::DeprecatedVector<3, T>;
using Vector3i = Vector3<int>;
using Vector3u = Vector3<unsigned>;
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

template<class T>
using Vector4 = Detail::DeprecatedVector<4, T>;
using Vector4i = Vector4<int>;
using Vector4u = Vector4<unsigned>;
using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;

template<size_t S, class T>
constexpr double get_distance(Detail::DeprecatedVector<S, T> const& a, Detail::DeprecatedVector<S, T> const& b) {
    return (a - b).length();
}

template<size_t S, class T>
    requires(S == 2 || S == 3)
constexpr double get_distance_to_line(Detail::DeprecatedVector<S, T> line_start, Detail::DeprecatedVector<S, T> line_end, Detail::DeprecatedVector<S, T> point) {
    auto d = (point - line_end) / get_distance(point, line_end);
    auto v = line_start - line_end;
    double t = v.dot(d);
    auto p = line_end + t * d;

    return get_distance(p, line_start);
}

}

template<size_t C, class T>
class fmt::formatter<Util::Detail::DeprecatedVector<C, T>> : public fmt::formatter<std::string_view> {
public:
    template<typename FormatContext>
    constexpr auto format(Util::Detail::DeprecatedVector<C, T> const& v, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "[");
        for (size_t s = 0; s < C; s++) {
            fmt::format_to(ctx.out(), "{}", v.components[s]);
            if (s != C - 1)
                fmt::format_to(ctx.out(), ", ");
        }
        return fmt::format_to(ctx.out(), "]");
        return ctx.out();
    }
};
