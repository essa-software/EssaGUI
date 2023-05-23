#pragma once

#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Rect.hpp>

namespace Gfx::Drawing {

class Fill {
public:
    static Fill none() { return Fill::solid(Util::Colors::Transparent); }
    static Fill solid(Util::Color const& color) { return Fill {}.set_color(color); }
    static Fill textured(llgl::Texture const& tex, Util::Rectf rect = {}) {
        return Fill {}.set_color(Util::Colors::White).set_texture(&tex).set_texture_rect(rect);
    }

    Fill& set_color(Util::Color color) {
        m_color = color;
        return *this;
    }
    Fill& set_texture(llgl::Texture const* tex) {
        m_texture = tex;
        return *this;
    }
    Fill& set_texture_rect(Util::Rectf rect) {
        m_texture_rect = rect;
        return *this;
    }

    Util::Color color() const { return m_color; }
    llgl::Texture const* texture() const { return m_texture; }
    Util::Rectf texture_rect() const { return m_texture_rect; }

    bool is_visible() const { return m_color.a > 0; }

private:
    Util::Color m_color;
    llgl::Texture const* m_texture = nullptr;
    Util::Rectf m_texture_rect;
};

}
