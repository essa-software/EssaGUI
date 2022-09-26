#pragma once

#include "DefaultGUIShader.hpp"
#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <LLGL/OpenGL/PrimitiveType.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/VertexArray.hpp>
#include <LLGL/Resources/TTFFont.hpp>
#include <LLGL/Window/Window.hpp>
#include <span>

namespace GUI {

struct DrawOptions {
    Util::Color fill_color = Util::Colors::White;
    Util::Color outline_color = Util::Colors::White;
    llgl::Texture const* texture = nullptr;
    Util::Rectu texture_rect;
    float outline_thickness = 0;
};

struct RectangleDrawOptions : public DrawOptions {
    float border_radius_top_left = 0;
    float border_radius_top_right = 0;
    float border_radius_bottom_right = 0;
    float border_radius_bottom_left = 0;

    void set_border_radius(float br) {
        border_radius_top_left = border_radius_top_right = border_radius_bottom_right = border_radius_bottom_left = br;
    }
};

class Window : public llgl::Window {
public:
    Window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& = {});
    virtual ~Window() = default;

    void set_projection(llgl::Projection p) { m_projection = p; }
    llgl::Projection projection() const { return m_projection; }

    void set_texture(llgl::Texture const* tex) { m_texture = tex; }

    void clear(Util::Color = Util::Colors::Black);

    void draw_vertices(llgl::PrimitiveType mode, std::span<Gfx::DefaultGUIShader::Vertex const>);
    void draw_outline(std::span<Util::Vector2f const>, Util::Color color, float thickness);
    void draw_indexed_vertices(llgl::PrimitiveType mode, std::span<Gfx::DefaultGUIShader::Vertex const>, std::span<unsigned const> indices);

    void draw_rectangle(Util::Rectf bounds, RectangleDrawOptions const& = {});
    void draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& = {});

private:
    void apply_states();

    llgl::VertexArray<Gfx::DefaultGUIShader::Vertex> m_temporary_vao;

    llgl::Projection m_projection;
    llgl::Texture const* m_texture = nullptr;
    Gfx::DefaultGUIShader m_default_shader;
};

}
