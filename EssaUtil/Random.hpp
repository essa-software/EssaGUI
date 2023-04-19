#pragma once

#include "CoordinateSystem.hpp"
#include "Rect.hpp"
#include <cmath>
#include <random>

namespace Util::Random {

template<class Random>
class Engine {
public:
    template<class... Args>
        requires requires(Args... args) { Random(args...); }
    Engine(Args&&... args)
        : m_std_random(std::forward<Args>(args)...) { }

    // Generate random bool, where 1/`chance` of bools will be true.
    bool next_bool(int chance) {
        return next_int(0, chance - 1) == 0;
    }

    // Generate random T ∈ ℤ ∩ <min; max>
    template<std::integral T>
    T next_int(T min, T max) {
        return std::uniform_int_distribution<T>(min, max)(m_std_random);
    }

    // Generate random T ∈ ℝ ∩ <min; max)
    template<std::floating_point T>
    T next_float(T min, T max) {
        return std::uniform_real_distribution<T>(min, max)(m_std_random);
    }

    // Generate random vector [ℝ×ℝ] of length [length]
    template<class T>
    Util::Cs::Vector2<T> next_vec2(float length) {
        double angle = next_float<T>(0, 2 * M_PI);
        double x;
        double y;
        sincos(angle, &x, &y);
        return Util::Cs::Vector2<T> { x, y } * length;
    }

    // Generate random point [x ∈ ℝ, y ∈ ℝ], where x ∈ <rect_left; rect_right) and y ∈ <rect_top; rect_bottom).
    template<std::floating_point T>
    Util::Cs::Point2<T> next_point2_in_rect(Util::Rect<T> rect) {
        T x = next_float(rect.left, rect.left + rect.width);
        T y = next_float(rect.top, rect.top + rect.height);
        return { x, y };
    }

    Random& std_random() { return m_std_random; }

private:
    Random m_std_random;
};

using DefaultEngine = Engine<std::default_random_engine>;

inline DefaultEngine& default_engine() {
    static DefaultEngine rng;
    return rng;
}

// Generate random T ∈ ℤ ∩ <min; max>
template<std::integral T>
T integer(T min, T max) {
    return default_engine().next_int<T>(min, max);
}

// Generate random T ∈ ℝ ∩ <min; max)
template<std::floating_point T>
T floating(T min, T max) {
    return default_engine().next_float<T>(min, max);
}

// Generate random vector [ℝ×ℝ] of length [length]
template<class T>
Util::Cs::Vector2<T> vector2(float length) {
    return default_engine().next_vec2<T>(length);
}

// Generate random vector [x ∈ ℝ, y ∈ ℝ], where x ∈ <rect_left; rect_right) and y ∈ <rect_top; rect_bottom).
template<std::floating_point T>
Util::Cs::Point2<T> point2_in_rect(Util::Rect<T> rect) {
    return default_engine().next_point2_in_rect<T>(rect);
}

}
