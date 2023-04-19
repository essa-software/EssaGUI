#pragma once

#include "../Vector.hpp"
#include "Coordinates.hpp"
#include <fmt/core.h>

namespace Util {

namespace Detail {

template<size_t C, class T>
class Point;

template<size_t C, class T>
class Size;

template<size_t C, class T>
class Vector : public Coordinates<C, T, Vector> {
public:
    using Super = Coordinates<C, T, Vector>;
    using ThisPoint = Point<C, T>;
    using ThisSize = Size<C, T>;

    // -> Interoperability
    ThisPoint to_point() const;
    ThisSize to_size() const;

    template<class... Args>
        requires requires(Args... a) { Super(std::forward<Args>(a)...); }
    Vector(Args... a)
        : Super(std::forward<Args>(a)...) { }

    template<class OtherT>
    static Vector from_deprecated_vector(DeprecatedVector<C, OtherT> const& c) {
        Vector p;
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

    auto length_squared() const {
        double result = 0;
        for (size_t s = 0; s < Super::Components; s++) {
            result += this->component(s) * this->component(s);
        }
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
        if (this->is_zero()) {
            return Vector {};
        }
        return *this * inverted_length();
    }

    bool is_normalized() const {
        auto length = length_squared();
        return std::abs(length - 1) < 10e-6 || length == 0;
    }

    double dot(Vector const& a) const {
        double result = 0;
        for (size_t s = 0; s < std::min<size_t>(Super::Components, 3); s++) {
            result += this->component(s) * a.component(s);
        }
        return result;
    }

    // Returns `this` resized to `length`, i.e this.normalized()*length.
    // If `this` is a null vector, this function returns null vector.
    constexpr Vector with_length(double length) const {
        if (this->is_zero()) {
            return Vector {};
        }
        return this->normalized() * length;
    }

    // Return vector with length being at most `length`
    constexpr Vector clamp_length(double length) const {
        auto this_length = this->length();
        if (this_length > length) {
            return this->normalized() * length;
        }
        return *this;
    }

    constexpr Vector operator+(Vector const& b) const {
        Vector ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) + b.component(s));
        }
        return ab;
    }

    constexpr Vector& operator+=(Vector const& b) {
        return *this = *this + b;
    }

    constexpr Vector operator-(Vector const& b) const {
        Vector ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) - b.component(s));
        }
        return ab;
    }

    constexpr Vector& operator-=(Vector const& b) {
        return *this = *this - b;
    }

    constexpr Vector operator*(T x) const {
        Vector ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) * x);
        }
        return ab;
    }

    constexpr Vector& operator*=(T x) {
        return *this = *this * x;
    }

    constexpr Vector operator/(T x) const {
        assert(x != 0);
        Vector ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, this->component(s) / x);
        }
        return ab;
    }

    constexpr Vector& operator/=(T x) {
        return *this = *this / x;
    }

    constexpr Vector operator-() const {
        Vector ab;
        for (size_t s = 0; s < Super::Components; s++) {
            ab.set_component(s, -this->component(s));
        }
        return ab;
    }

    //// Vector2 ////
    template<size_t OtherC, class OtherT>
        requires(Super::Components == 2 && OtherC >= 2)
    constexpr explicit Vector(Vector<OtherC, OtherT> const& other)
        : Vector { other.x(), other.y() } {
    }

    // Angle is CCW starting from positive X axis.
    constexpr static Vector create_polar(Angle angle, double length)
        requires(Super::Components == 2)
    {
        return { std::cos(angle.rad()) * length, std::sin(angle.rad()) * length };
    }

    // Calculate the shortest (acute) directed angle between X axis and this vector, in range <-π; π>
    constexpr Angle angle() const
        requires(Super::Components == 2)
    {
        return Angle::radians(std::atan2(this->y(), this->x()));
    }

    // Calculate the shortest (acute) directed angle from `this` to `other`, in range <-π; π>.
    constexpr Angle directed_angle_to(Vector<2, T> const& other) const
        requires(Super::Components == 2)
    {
        auto angle1 = this->angle();
        auto angle2 = other.angle();
        return Angle::radians(std::remainder(angle2.rad() - angle1.rad(), 2 * M_PI));
    }

    template<class OtherT = T>
        requires(Super::Components == 2)
    constexpr Vector<2, OtherT> rotate(Angle theta) const {
        double t_cos = std::cos(theta.rad()), t_sin = std::sin(theta.rad());
        return { this->x() * t_cos - this->y() * t_sin, this->x() * t_sin + this->y() * t_cos };
    }

    template<class OtherT = T>
        requires(Super::Components == 2)
    constexpr auto perpendicular() const {
        return Vector<2, OtherT> { -this->y(), this->x() };
    }

    template<class OtherT>
        requires(Super::Components == 2)
    constexpr auto mirror_against_tangent(Vector<2, OtherT> const& tangent) const {
        assert(tangent.is_normalized());
        return *this - tangent * T(2) * dot(tangent);
    }

    template<class OtherT>
        requires(Super::Components == 2)
    constexpr auto reflect_against_tangent(Vector<2, OtherT> const& tangent) const {
        return -mirror_against_tangent(tangent);
    }

    // AKA Godot's slide()
    template<class OtherT>
        requires(Super::Components == 2)
    constexpr auto normal_part(Vector<2, OtherT> const& plane_normal) const {
        assert(plane_normal.is_normalized());
        return *this - plane_normal * dot(plane_normal);
    }

    template<class OtherT>
        requires(Super::Components == 2)
    constexpr auto tangent_part(Vector<2, OtherT> const& plane_normal) const {
        return *this - normal_part(plane_normal);
    }

    //// Vector3 ////
    template<size_t OtherC, class OtherT>
        requires(Super::Components == 3 && OtherC >= 3)
    constexpr explicit Vector(Vector<OtherC, OtherT> const& other)
        : Vector { other.x(), other.y(), other.z() } {
    }

    constexpr static Vector create_spheric(Angle lat, Angle lon, double radius)
        requires(Super::Components == 3)
    {
        return {
            static_cast<T>(radius * std::cos(lat.rad()) * std::sin(lon.rad())),
            static_cast<T>(radius * std::sin(lat.rad()) * std::sin(lon.rad())),
            static_cast<T>(radius * std::cos(lon.rad())),
        };
    }

    template<class OtherT = T>
        requires(Super::Components == 3)
    constexpr Vector<3, OtherT> rotate_x(Angle theta) const {
        double t_cos = std::cos(theta.rad()), t_sin = std::sin(theta.rad());
        return { this->x(), this->y() * t_cos - this->z() * t_sin, this->y() * t_sin + this->z() * t_cos };
    }

    template<class OtherT = T>
        requires(Super::Components == 3)
    constexpr Vector<3, OtherT> rotate_y(Angle theta) const {
        double t_cos = std::cos(theta.rad()), t_sin = std::sin(theta.rad());
        return { this->x() * t_cos - this->z() * t_sin, this->y(), this->x() * t_sin + this->z() * t_cos };
    }

    template<class OtherT = T>
        requires(Super::Components == 3)
    constexpr Vector<3, OtherT> rotate_z(Angle theta) const {
        double t_cos = std::cos(theta.rad()), t_sin = std::sin(theta.rad());
        return { this->x() * t_cos - this->y() * t_sin, this->x() * t_sin + this->y() * t_cos, this->z() };
    }

    template<class OtherT = T>
        requires(Super::Components == 3)
    constexpr Vector<3, OtherT> cross(Vector<3, T> const& a) const {
        Vector<3, OtherT> result;
        result.set_x(this->y() * a.z() - this->z() * a.y());
        result.set_y(this->z() * a.x() - this->x() * a.z());
        result.set_z(this->x() * a.y() - this->y() * a.x());
        return result;
    }

    //// Vector4 ////

    template<size_t OtherC, class OtherT>
        requires(Super::Components == 4 && OtherC >= 4)
    constexpr explicit Vector(Vector<OtherC, OtherT> other)
        : Vector { other.x(), other.y(), other.z(), other.w() } {
    }

    bool operator==(Vector const&) const = default;
};

} // Detail

template<size_t C, class T>
Detail::Vector<C, T> operator*(double fac, Detail::Vector<C, T> const& vec) {
    return vec * fac;
}

} // Util

template<size_t C, class T>
class fmt::formatter<Util::Detail::Vector<C, T>> : public fmt::formatter<std::string_view> {
public:
    template<typename FormatContext>
    constexpr auto format(Util::Detail::Vector<C, T> const& v, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "[");
        for (size_t s = 0; s < C; s++) {
            fmt::format_to(ctx.out(), "{}", v.component(s));
            if (s != C - 1)
                fmt::format_to(ctx.out(), ", ");
        }
        return fmt::format_to(ctx.out(), "]");
        return ctx.out();
    }
};
