#pragma once

#include "Config.hpp"
#include "UString.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>

namespace Util {

struct Colorf;
struct HSV;

class Color {
public:
    uint8_t r {}, g {}, b {}, a { 255 };

    constexpr Color() = default;

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r)
        , g(g)
        , b(b)
        , a(a) { }

    constexpr Color(uint32_t color)
        : r((color >> 24) & 0xff)
        , g((color >> 16) & 0xff)
        , b((color >> 8) & 0xff)
        , a((color >> 0) & 0xff) { }

    constexpr Color(Colorf const&);

    constexpr Color operator+(Color const& right) const {
        return { static_cast<uint8_t>(std::clamp(
                     static_cast<int>(r) + static_cast<int>(right.r), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(g) + static_cast<int>(right.g), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(b) + static_cast<int>(right.b), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(a) + static_cast<int>(right.a), 0, 255)) };
    }

    constexpr Color operator-(Color const& right) const {
        return { static_cast<uint8_t>(std::clamp(
                     static_cast<int>(r) - static_cast<int>(right.r), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(g) - static_cast<int>(right.g), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(b) - static_cast<int>(right.b), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(a) - static_cast<int>(right.a), 0, 255)) };
    }

    constexpr HSV to_hsv() const;
    constexpr Color to_grayscale(float saturation = 0) const;

    friend std::ostream& operator<<(std::ostream& out, Color const& color) {
        return out << "rgba(" << static_cast<int>(color.r) << ", "
                   << static_cast<int>(color.g) << ", " << static_cast<int>(color.b)
                   << ", " << static_cast<int>(color.a) << ")";
    }

    Util::UString to_html_string() const;

    Util::Color with_red(uint8_t red) const { return Util::Color { red, this->g, this->b, this->a }; }
    Util::Color with_green(uint8_t green) const { return Util::Color { this->r, green, this->b, this->a }; }
    Util::Color with_blue(uint8_t blue) const { return Util::Color { this->r, this->g, blue, this->a }; }
    Util::Color with_alpha(uint8_t alpha) const { return Util::Color { this->r, this->g, this->b, alpha }; }

    bool operator==(Color const&) const = default;

    static Color rgb_blend(Color const& l, Color const& r, float how_much_r);
};

struct HSV {
    int hue;          // 0 - 360
    float saturation; // 0 - 1
    float value;      // 0 - 1

    constexpr Color to_rgb() const;
};

constexpr Color operator*(Color const& left, auto fac) {
    using Fac = decltype(fac);
    return { static_cast<uint8_t>(std::min<Fac>(255, left.r * fac)),
        static_cast<uint8_t>(std::min<Fac>(255, left.g * fac)),
        static_cast<uint8_t>(std::min<Fac>(255, left.b * fac)), left.a };
}

class Colors {
public:
    static constexpr Color Transparent = Color(0x00000000);
    static constexpr Color AliceBlue = Color(0xf0f8ffff);
    static constexpr Color AntiqueWhite = Color(0xfaebd7ff);
    static constexpr Color Aqua = Color(0x00ffffff);
    static constexpr Color Aquamarine = Color(0x7fffd4ff);
    static constexpr Color Azure = Color(0xf0ffffff);
    static constexpr Color Beige = Color(0xf5f5dcff);
    static constexpr Color Bisque = Color(0xffe4c4ff);
    static constexpr Color Black = Color(0x000000ff);
    static constexpr Color BlanchedAlmond = Color(0xffebcdff);
    static constexpr Color Blue = Color(0x0000ffff);
    static constexpr Color BlueViolet = Color(0x8a2be2ff);
    static constexpr Color Brown = Color(0xa52a2aff);
    static constexpr Color BurlyWood = Color(0xdeb887ff);
    static constexpr Color CadetBlue = Color(0x5f9ea0ff);
    static constexpr Color Chartreuse = Color(0x7fff00ff);
    static constexpr Color Chocolate = Color(0xd2691eff);
    static constexpr Color Coral = Color(0xff7f50ff);
    static constexpr Color CornflowerBlue = Color(0x6495edff);
    static constexpr Color Cornsilk = Color(0xfff8dcff);
    static constexpr Color Crimson = Color(0xdc143cff);
    static constexpr Color Cyan = Color(0x00ffffff);
    static constexpr Color DarkBlue = Color(0x00008bff);
    static constexpr Color DarkCyan = Color(0x008b8bff);
    static constexpr Color DarkGoldenRod = Color(0xb8860bff);
    static constexpr Color DarkGray = Color(0xa9a9a9ff);
    static constexpr Color DarkGrey = Color(0xa9a9a9ff);
    static constexpr Color DarkGreen = Color(0x006400ff);
    static constexpr Color DarkKhaki = Color(0xbdb76bff);
    static constexpr Color DarkMagenta = Color(0x8b008bff);
    static constexpr Color DarkOliveGreen = Color(0x556b2fff);
    static constexpr Color DarkOrange = Color(0xff8c00ff);
    static constexpr Color DarkOrchid = Color(0x9932ccff);
    static constexpr Color DarkRed = Color(0x8b0000ff);
    static constexpr Color DarkSalmon = Color(0xe9967aff);
    static constexpr Color DarkSeaGreen = Color(0x8fbc8fff);
    static constexpr Color DarkSlateBlue = Color(0x483d8bff);
    static constexpr Color DarkSlateGray = Color(0x2f4f4fff);
    static constexpr Color DarkSlateGrey = Color(0x2f4f4fff);
    static constexpr Color DarkTurquoise = Color(0x00ced1ff);
    static constexpr Color DarkViolet = Color(0x9400d3ff);
    static constexpr Color DeepPink = Color(0xff1493ff);
    static constexpr Color DeepSkyBlue = Color(0x00bfffff);
    static constexpr Color DimGray = Color(0x696969ff);
    static constexpr Color DimGrey = Color(0x696969ff);
    static constexpr Color DodgerBlue = Color(0x1e90ffff);
    static constexpr Color FireBrick = Color(0xb22222ff);
    static constexpr Color FloralWhite = Color(0xfffaf0ff);
    static constexpr Color ForestGreen = Color(0x228b22ff);
    static constexpr Color Fuchsia = Color(0xff00ffff);
    static constexpr Color Gainsboro = Color(0xdcdcdcff);
    static constexpr Color GhostWhite = Color(0xf8f8ffff);
    static constexpr Color Gold = Color(0xffd700ff);
    static constexpr Color GoldenRod = Color(0xdaa520ff);
    static constexpr Color Gray = Color(0x808080ff);
    static constexpr Color Grey = Color(0x808080ff);
    static constexpr Color Green = Color(0x008000ff);
    static constexpr Color GreenYellow = Color(0xadff2fff);
    static constexpr Color HoneyDew = Color(0xf0fff0ff);
    static constexpr Color HotPink = Color(0xff69b4ff);
    static constexpr Color IndianRed = Color(0xcd5c5cff);
    static constexpr Color Indigo = Color(0x4b0082ff);
    static constexpr Color Ivory = Color(0xfffff0ff);
    static constexpr Color Khaki = Color(0xf0e68cff);
    static constexpr Color Lavender = Color(0xe6e6faff);
    static constexpr Color LavenderBlush = Color(0xfff0f5ff);
    static constexpr Color LawnGreen = Color(0x7cfc00ff);
    static constexpr Color LemonChiffon = Color(0xfffacdff);
    static constexpr Color LightBlue = Color(0xadd8e6ff);
    static constexpr Color LightCoral = Color(0xf08080ff);
    static constexpr Color LightCyan = Color(0xe0ffffff);
    static constexpr Color LightGoldenRodYellow = Color(0xfafad2ff);
    static constexpr Color LightGray = Color(0xd3d3d3ff);
    static constexpr Color LightGrey = Color(0xd3d3d3ff);
    static constexpr Color LightGreen = Color(0x90ee90ff);
    static constexpr Color LightPink = Color(0xffb6c1ff);
    static constexpr Color LightSalmon = Color(0xffa07aff);
    static constexpr Color LightSeaGreen = Color(0x20b2aaff);
    static constexpr Color LightSkyBlue = Color(0x87cefaff);
    static constexpr Color LightSlateGray = Color(0x778899ff);
    static constexpr Color LightSlateGrey = Color(0x778899ff);
    static constexpr Color LightSteelBlue = Color(0xb0c4deff);
    static constexpr Color LightYellow = Color(0xffffe0ff);
    static constexpr Color Lime = Color(0x00ff00ff);
    static constexpr Color LimeGreen = Color(0x32cd32ff);
    static constexpr Color Linen = Color(0xfaf0e6ff);
    static constexpr Color Magenta = Color(0xff00ffff);
    static constexpr Color Maroon = Color(0x800000ff);
    static constexpr Color MediumAquaMarine = Color(0x66cdaaff);
    static constexpr Color MediumBlue = Color(0x0000cdff);
    static constexpr Color MediumOrchid = Color(0xba55d3ff);
    static constexpr Color MediumPurple = Color(0x9370dbff);
    static constexpr Color MediumSeaGreen = Color(0x3cb371ff);
    static constexpr Color MediumSlateBlue = Color(0x7b68eeff);
    static constexpr Color MediumSpringGreen = Color(0x00fa9aff);
    static constexpr Color MediumTurquoise = Color(0x48d1ccff);
    static constexpr Color MediumVioletRed = Color(0xc71585ff);
    static constexpr Color MidnightBlue = Color(0x191970ff);
    static constexpr Color MintCream = Color(0xf5fffaff);
    static constexpr Color MistyRose = Color(0xffe4e1ff);
    static constexpr Color Moccasin = Color(0xffe4b5ff);
    static constexpr Color NavajoWhite = Color(0xffdeadff);
    static constexpr Color Navy = Color(0x000080ff);
    static constexpr Color OldLace = Color(0xfdf5e6ff);
    static constexpr Color Olive = Color(0x808000ff);
    static constexpr Color OliveDrab = Color(0x6b8e23ff);
    static constexpr Color Orange = Color(0xffa500ff);
    static constexpr Color OrangeRed = Color(0xff4500ff);
    static constexpr Color Orchid = Color(0xda70d6ff);
    static constexpr Color PaleGoldenRod = Color(0xeee8aaff);
    static constexpr Color PaleGreen = Color(0x98fb98ff);
    static constexpr Color PaleTurquoise = Color(0xafeeeeff);
    static constexpr Color PaleVioletRed = Color(0xdb7093ff);
    static constexpr Color PapayaWhip = Color(0xffefd5ff);
    static constexpr Color PeachPuff = Color(0xffdab9ff);
    static constexpr Color Peru = Color(0xcd853fff);
    static constexpr Color Pink = Color(0xffc0cbff);
    static constexpr Color Plum = Color(0xdda0ddff);
    static constexpr Color PowderBlue = Color(0xb0e0e6ff);
    static constexpr Color Purple = Color(0x800080ff);
    static constexpr Color RebeccaPurple = Color(0x663399ff);
    static constexpr Color Red = Color(0xff0000ff);
    static constexpr Color RosyBrown = Color(0xbc8f8fff);
    static constexpr Color RoyalBlue = Color(0x4169e1ff);
    static constexpr Color SaddleBrown = Color(0x8b4513ff);
    static constexpr Color Salmon = Color(0xfa8072ff);
    static constexpr Color SandyBrown = Color(0xf4a460ff);
    static constexpr Color SeaGreen = Color(0x2e8b57ff);
    static constexpr Color SeaShell = Color(0xfff5eeff);
    static constexpr Color Sienna = Color(0xa0522dff);
    static constexpr Color Silver = Color(0xc0c0c0ff);
    static constexpr Color SkyBlue = Color(0x87ceebff);
    static constexpr Color SlateBlue = Color(0x6a5acdff);
    static constexpr Color SlateGray = Color(0x708090ff);
    static constexpr Color SlateGrey = Color(0x708090ff);
    static constexpr Color Snow = Color(0xfffafaff);
    static constexpr Color SpringGreen = Color(0x00ff7fff);
    static constexpr Color SteelBlue = Color(0x4682b4ff);
    static constexpr Color Tan = Color(0xd2b48cff);
    static constexpr Color Teal = Color(0x008080ff);
    static constexpr Color Thistle = Color(0xd8bfd8ff);
    static constexpr Color Tomato = Color(0xff6347ff);
    static constexpr Color Turquoise = Color(0x40e0d0ff);
    static constexpr Color Violet = Color(0xee82eeff);
    static constexpr Color Wheat = Color(0xf5deb3ff);
    static constexpr Color White = Color(0xffffffff);
    static constexpr Color WhiteSmoke = Color(0xf5f5f5ff);
    static constexpr Color Yellow = Color(0xffff00ff);
    static constexpr Color YellowGreen = Color(0x9acd32ff);
};

struct Colorf {
    constexpr Colorf() = default;
    constexpr Colorf(float r_, float g_, float b_, float a_ = 1)
        : r(r_)
        , g(g_)
        , b(b_)
        , a(a_) { }

    constexpr Colorf(Color const& color)
        : r(color.r / 255.f)
        , g(color.g / 255.f)
        , b(color.b / 255.f)
        , a(color.a / 255.f) { }

    float r, g, b, a;

    bool operator==(Colorf const&) const = default;
};

constexpr Color HSV::to_rgb() const {
    auto c = value * saturation;
    auto x = c * (1 - std::abs(std::fmod(hue / 60.f, 2.f) - 1));
    auto m = value - c;
    Colorf color = [&]() -> Colorf {
        if (hue < 60)
            return { c, x, 0 };
        if (hue < 120)
            return { x, c, 0 };
        if (hue < 180)
            return { 0, c, x };
        if (hue < 240)
            return { 0, x, c };
        if (hue < 300)
            return { x, 0, c };
        return { c, 0, x };
    }();
    color.r += m;
    color.g += m;
    color.b += m;
    return color;
}

constexpr Color::Color(Colorf const& color)
    : r(color.r * 255)
    , g(color.g * 255)
    , b(color.b * 255)
    , a(color.a * 255) { }

constexpr HSV Color::to_hsv() const {
    Colorf p { *this };
    auto cmax = std::max(p.r, std::max(p.g, p.b));
    auto cmin = std::min(p.r, std::min(p.g, p.b));
    auto delta = cmax - cmin;
    HSV hsv;
    hsv.hue = [&]() -> double {
        if (delta == 0)
            return 0;
        if (cmax == p.r)
            return 60 * std::fmod((p.g - p.b) / delta, 6);
        if (cmax == p.g)
            return 60 * ((p.b - p.r) / delta + 2);
        if (cmax == p.b)
            return 60 * ((p.r - p.g) / delta + 4);
        ESSA_UNREACHABLE;
    }();
    if (hsv.hue < 0)
        hsv.hue += 360;
    hsv.saturation = cmax == 0 ? 0 : delta / cmax;
    hsv.value = cmax;
    return hsv;
}

constexpr Color Color::to_grayscale(float saturation) const {
    auto hsv = to_hsv();
    hsv.saturation = std::clamp<float>(saturation, 0, 1);
    return hsv.to_rgb();
}

} // namespace Util

template<>
class fmt::formatter<Util::Color> : public fmt::formatter<std::string_view> {
public:
    template<typename FormatContext>
    constexpr auto format(Util::Color const& v, FormatContext& ctx) const {
        if (v.a == 255) {
            fmt::format_to(ctx.out(), "rgb({}, {}, {})", v.r, v.g, v.b);
        }
        else {
            fmt::format_to(ctx.out(), "rgba({}, {}, {}, {})", v.r, v.g, v.b, v.a / 255.0);
        }
        return ctx.out();
    }
};
