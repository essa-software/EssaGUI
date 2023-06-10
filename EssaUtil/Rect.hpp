#pragma once

#include <concepts>
#include <fmt/core.h>

#include "CoordinateSystem.hpp"

namespace Util {

template<class T> class Rect {
public:
    T left {};
    T top {};
    T width {};
    T height {};

    Rect() = default;

    Rect(T left_, T top_, T width_, T height_)
        : left(left_)
        , top(top_)
        , width(width_)
        , height(height_) { }

    Rect(Point2<T> position, Size2<T> size)
        : left(position.x())
        , top(position.y())
        , width(size.x())
        , height(size.y()) { }

    template<class U>
    explicit Rect(Rect<U> const& other)
        : left(other.left)
        , top(other.top)
        , width(other.width)
        , height(other.height) { }

    template<class U> Rect<U> cast() const {
        return Rect<U>(static_cast<U>(left), static_cast<U>(top), static_cast<U>(width), static_cast<U>(height));
    }

    static Rect centered(Point2<T> center, Size2<T> size) { return Rect { center - size.to_vector() / T { 2 }, size }; }

    Point2<T> position() const { return { left, top }; }
    Size2<T> size() const { return { width, height }; }
    Point2<T> center() const { return { left + width / 2, top + height / 2 }; }
    Point2<T> bottom_right() const { return { left + width, top + height }; }

    template<class U = T> bool contains(Point2<U> pos) const {
        return pos.x() >= left && pos.x() <= left + width && pos.y() >= top && pos.y() <= top + height;
    }

    template<class U = T> Rect<U> intersection(Rect<U> const& other) const {
        auto fixed_this = with_negative_size_fixed();
        auto fixed_other = other.with_negative_size_fixed();

        T max_x = std::max<U>(fixed_this.left, fixed_other.left);
        T max_y = std::max<U>(fixed_this.top, fixed_other.top);

        T min_x = std::min<U>(fixed_this.left + fixed_this.width, fixed_other.left + fixed_other.width);
        T min_y = std::min<U>(fixed_this.top + fixed_this.height, fixed_other.top + fixed_other.height);

        return max_x < min_x && max_y < min_y ? Rect<U>({ max_x, max_y }, { min_x - max_x, min_y - max_y }) : Rect<U>();
    }

    template<class U = T> Rect<U> componentwise_divide(Size2<T> const& size) const {
        return {
            static_cast<U>(left) / size.x(),
            static_cast<U>(top) / size.y(),
            static_cast<U>(width) / size.x(),
            static_cast<U>(height) / size.y(),
        };
    }

    Rect<T> move_x(T t) const { return Rect { left + t, top, width, height }; }
    Rect<T> move_y(T t) const { return Rect { left, top + t, width, height }; }
    Rect<T> take_top(T t) const { return Rect { left, top, width, t }; }
    Rect<T> take_right(T t) const { return Rect { left + width - t, top, t, height }; }
    Rect<T> take_bottom(T t) const { return Rect { left, top + height - t, width, t }; }
    Rect<T> take_left(T t) const { return Rect { left, top, t, height }; }
    // Return rect with all sides moved by `t` outside
    Rect<T> inflated(T t) const { return Rect { left - t, top - t, width + 2 * t, height + 2 * t }; }
    // Return rect with top and bottom sides moved by `t` outside
    Rect<T> inflated_vertical(T t) const { return Rect { left, top - t, width, height + 2 * t }; }
    // Return rect with left and right sides moved by `t` outside
    Rect<T> inflated_horizontal(T t) const { return Rect { left - t, top, width + 2 * t, height }; }

    // Fix rect with negative size by moving it's position and making size positive
    Rect with_negative_size_fixed() const {
        T new_left = left;
        T new_top = top;
        T new_width = width;
        T new_height = height;

        if (width < 0) {
            new_left += width;
            new_width = -width;
        }

        if (height < 0) {
            new_top += height;
            new_height = -height;
        }

        return Rect { new_left, new_top, new_width, new_height };
    }

    bool operator==(Rect<T> const&) const = default;
};

using Recti = Rect<int>;
using Rectu = Rect<unsigned>;
using Rectf = Rect<float>;
using Rectd = Rect<double>;

}

template<class T> class fmt::formatter<Util::Rect<T>> : public fmt::formatter<std::string_view> {
public:
    template<typename FormatContext> constexpr auto format(Util::Rect<T> const& p, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "Rect[@{} {}x{}]", p.position(), p.size().x(), p.size().y());
        return ctx.out();
    }
};
