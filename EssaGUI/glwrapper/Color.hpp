#pragma once

#include <GL/gl.h>
#include <SFML/Graphics/Color.hpp>
#include <cstdint>

// FIXME: This doesn't differ from sf::Color in any way. Even has converting function...
class Color {
public:
    uint8_t r, g, b, a;
    Color()
        : r(0)
        , g(0)
        , b(0)
        , a(255) { }

    Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
        : r(_r)
        , g(_g)
        , b(_b)
        , a(_a) { }

    Color(const sf::Color color)
        : r(color.r)
        , g(color.g)
        , b(color.b)
        , a(color.a) { }

    operator sf::Color() {
        return sf::Color(r, g, b, a);
    }

    Color operator+(const Color& color) const {
        Color result(*this);
        result.r = result.r + color.r;
        result.g = result.g + color.g;
        result.b = result.b + color.b;
        result.a = result.a + color.a;

        return result;
    }

    Color operator*(const double mul) const {
        Color result(*this);
        result.r *= mul;
        result.g *= mul;
        result.b *= mul;
        result.a *= mul;

        return result;
    }

    Color& operator+=(const Color& color) {
        *this = *this + color;

        return *this;
    }

    Color& operator*=(const double mul) {
        *this = *this * mul;

        return *this;
    }

    void glDraw() const {
        glColor4f((float)r / 255, (float)g / 255, (float)b / 255, (float)a / 255);
    }
};
