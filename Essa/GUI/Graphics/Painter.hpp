#pragma once

#include "DefaultGUIShader.hpp"
#include "GUIBuilder.hpp"
#include <Essa/LLGL/OpenGL/Builder.hpp>

namespace llgl {
class Texture;
}

namespace Gfx {

namespace Drawing {
class Shape;
}

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

    void render() const {
        apply_states();
        m_builder.render(m_renderer);
    }

    void draw(Drawing::Shape const&);

    void deprecated_draw_rectangle(Util::Rectf bounds, RectangleDrawOptions const& = {});
    /*deprecated: use draw(Ellipse(...))*/ void draw_ellipse(Util::Point2f center, Util::Size2f size, DrawOptions const& = {});
    void draw_line(std::span<Util::Point2f const>, LineDrawOptions const&);
    void draw_line(std::initializer_list<Util::Point2f> vertices, LineDrawOptions const& options) {
        draw_line(std::span { vertices }, options);
    }
    void draw_line(Util::Point2f start, Util::Point2f end, LineDrawOptions const& options) { draw_line({ start, end }, options); }

    void draw_outline(std::span<Util::Point2f const>, Util::Color color, float thickness);
    void draw_vertices(
        llgl::PrimitiveType type, std::span<Gfx::DefaultGUIShader::Vertex const>, llgl::Texture const* = nullptr,
        std::optional<ShaderContext> shader_context = {}
    );

    void reset() { m_builder.reset(); }

    enum class BlendingFunc {
        Zero = GL_ZERO,
        One = GL_ONE,
        SrcColor = GL_SRC_COLOR,
        OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
        DstColor = GL_DST_COLOR,
        OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DstAlpha = GL_DST_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor = GL_CONSTANT_COLOR,
        OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
        SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE,
        Src1Color = GL_SRC1_COLOR,
        OneMinusSrc1Color = GL_ONE_MINUS_SRC1_COLOR,
        Src1Alpha = GL_SRC1_ALPHA,
        OneMinusSrc1Alpha = GL_ONE_MINUS_SRC1_ALPHA,
    };

    struct Blending {
        BlendingFunc src_rgb;
        BlendingFunc dst_rgb;
        BlendingFunc src_alpha;
        BlendingFunc dst_alpha;
    };
    // Set blending used for rendering. NOTE/FIXME: This is applied only
    // on actual rendering, not on individual draw() calls.
    void set_blending(Blending blending) { m_blending = blending; }
    Blending blending() const { return m_blending; }

private:
    void apply_states() const;

    // In these functions, vertices are and after rounding, but not transformed.
    void draw_fill(Drawing::Shape const& shape, std::span<Util::Point2f const> vertices, std::optional<ShaderContext> shader_context = {});
    void draw_outline(Drawing::Shape const& shape, std::span<Util::Point2f const> vertices);

    GUIBuilder m_builder;
    llgl::Renderer& m_renderer;
    Blending m_blending {
        .src_rgb = BlendingFunc::SrcAlpha,
        .dst_rgb = BlendingFunc::OneMinusSrcAlpha,
        .src_alpha = BlendingFunc::SrcAlpha,
        .dst_alpha = BlendingFunc::OneMinusSrcAlpha,
    };
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

    ~PainterTransformScope() { m_painter.builder().set_view(m_old_transform); }

private:
    Gfx::Painter& m_painter;
    llgl::Transform m_old_transform;
};

}
