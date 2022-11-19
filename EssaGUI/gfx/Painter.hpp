#pragma once

#include "DefaultGUIShader.hpp"
#include "GUIBuilder.hpp"
#include "Vertex.hpp"
#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaUtil/NonCopyable.hpp>
#include <LLGL/OpenGL/Texture.hpp>

namespace Gfx {

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

struct LineDrawOptions {
    Util::Color color;
};

class Painter {
public:
    Painter(llgl::Renderer& renderer)
        : m_renderer(renderer) { }

    auto& builder() const { return m_builder; }
    auto& builder() { return m_builder; }
    auto& renderer() const { return m_renderer; }

    void render() {
        apply_states();
        m_builder.render(m_renderer);
    }

    void draw_rectangle(Util::Rectf bounds, RectangleDrawOptions const& = {});
    void draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& = {});
    void draw_line(std::span<Util::Vector2f const>, LineDrawOptions const&);
    void draw_line(std::initializer_list<Util::Vector2f> vertices, LineDrawOptions const& options) {
        draw_line(std::span { vertices }, options);
    }
    void draw_line(Util::Vector2f start, Util::Vector2f end, LineDrawOptions const& options) {
        draw_line({ start, end }, options);
    }

    void draw_outline(std::span<Util::Vector2f const>, Util::Color color, float thickness);
    void draw_vertices(llgl::PrimitiveType mode, std::span<Gfx::DefaultGUIShader::Vertex const>, llgl::Texture const* = nullptr);

    void reset() {
        m_builder.reset();
    }

private:
    void apply_states();

    GUIBuilder m_builder;
    llgl::Renderer& m_renderer;
};

class PainterTransformScope {
public:
    PainterTransformScope(PainterTransformScope const&) = delete;
    PainterTransformScope& operator=(PainterTransformScope const&) = delete;
    PainterTransformScope(PainterTransformScope&&) = delete;
    PainterTransformScope& operator=(PainterTransformScope&&) = delete;

    PainterTransformScope(Gfx::Painter& painter, llgl::Transform const& transform)
        : m_painter(painter)
        , m_old_transform(painter.builder().view()) {
        painter.builder().set_view(transform);
    }

    ~PainterTransformScope() {
        m_painter.builder().set_view(m_old_transform);
    }

private:
    Gfx::Painter& m_painter;
    llgl::Transform m_old_transform;
};

}
