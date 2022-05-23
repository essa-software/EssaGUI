#pragma once

#include <SFML/System.hpp>
#include <cmath>
#include <ostream>

#if defined(__GNUC__) || defined(__clang__)
#    define ALWAYS_INLINE [[gnu::always_inline]]
#endif

class Vector3 {
public:
    double x = 0, y = 0, z = 0, w = 1;

    ALWAYS_INLINE Vector3() = default;

    ALWAYS_INLINE Vector3(double _x, double _y, double _z = 0, double _w = 1)
        : x(_x)
        , y(_y)
        , z(_z)
        , w(_w) { }
    ALWAYS_INLINE explicit Vector3(sf::Vector2f _a)
        : x(_a.x)
        , y(_a.y)
        , z(0) { }
    ALWAYS_INLINE explicit Vector3(sf::Vector2u _a)
        : x(_a.x)
        , y(_a.y)
        , z(0) { }
    ALWAYS_INLINE explicit Vector3(sf::Vector2i _a)
        : x(_a.x)
        , y(_a.y)
        , z(0) { }

    sf::Vector2f to_sfml_vector() const { return { static_cast<float>(x), static_cast<float>(y) }; }

    double magnitude() const;
    double magnitude_squared() const;
    double inverted_magnitude() const;
    double angle() const;

    void glDraw() const;

    // Return a vector rotated by theta around (0,1,0) axis.
    // FIXME: Support arbitrary axis when this is needed.
    Vector3 rotate_vector(double theta) const;

    double distance_to(Vector3 const& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    Vector3 normalized() const;
    double dot(const Vector3& a) const;
    Vector3 cross(const Vector3& a) const;

    friend std::ostream& operator<<(std::ostream& out, const Vector3& vec) {
        return out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    }
};

inline Vector3 operator+(const Vector3& a, const Vector3& b) { return Vector3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline Vector3 operator-(const Vector3& a, const Vector3& b) { return Vector3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vector3 operator*(const Vector3& a, double b) { return Vector3(a.x * b, a.y * b, a.z * b); }
inline Vector3 operator*(double a, const Vector3& b) { return Vector3(a * b.x, a * b.y, a * b.z); }
inline Vector3 operator/(const Vector3& a, double b) { return Vector3(a.x / b, a.y / b, a.z / b); }
inline Vector3 operator-(const Vector3& a) { return Vector3(-a.x, -a.y, -a.z); }

inline Vector3& operator+=(Vector3& a, const Vector3& b) { return a = a + b; }
inline Vector3& operator-=(Vector3& a, const Vector3& b) { return a = a - b; }
inline Vector3& operator*=(Vector3& a, double b) { return a = a * b; }
inline Vector3& operator/=(Vector3& a, double b) { return a = a / b; }

inline bool operator==(const Vector3& a, const Vector3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
inline bool operator!=(const Vector3& a, const Vector3& b) { return !(a == b); }

inline double Vector3::magnitude() const {
    return std::sqrt(magnitude_squared());
}

inline double Vector3::inverted_magnitude() const {
    return 1 / magnitude();
}

inline double Vector3::magnitude_squared() const {
    return x * x + y * y + z * z;
}

inline double Vector3::angle() const {
    return (std::atan2(y, x));
}

inline Vector3 Vector3::rotate_vector(double theta) const {
    double t_cos = std::cos(theta), t_sin = std::sin(theta);

    return Vector3(x * t_cos - y * t_sin, x * t_sin + y * t_cos, z);
}

inline Vector3 Vector3::normalized() const {
    return Vector3(x, y, z) * inverted_magnitude();
}

inline double Vector3::dot(const Vector3& a) const {
    return this->x * a.x + this->y * a.y + this->z * a.z;
}

inline Vector3 Vector3::cross(const Vector3& a) const {
    Vector3 result;
    result.x = this->y * a.z - this->z * a.y;
    result.y = this->z * a.x - this->x * a.z;
    result.z = this->x * a.y - this->y * a.x;

    return result;
}

inline double get_distance(Vector3 a, Vector3 b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

inline double get_distance_to_line(Vector3 line_start, Vector3 line_end, Vector3 point) {
    auto d = (point - line_end) / get_distance(point, line_end);
    auto v = line_start - line_end;
    double t = v.dot(d);
    auto p = line_end + t * d;

    return get_distance(p, line_start);
}
