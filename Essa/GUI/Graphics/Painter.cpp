#include "Painter.hpp"

#include <Essa/GUI/Graphics/Drawing/Ellipse.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Drawing/Shape.hpp>
#include <Essa/GUI/Graphics/Vertex.hpp>
#include <Essa/LLGL/OpenGL/PrimitiveType.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Config.hpp>
#include <algorithm>
#include <fmt/ostream.h>
#include <ranges>

namespace Gfx {

void Painter::draw_fill(Drawing::Shape const& shape, std::span<Util::Point2f const> vertices, std::optional<ShaderContext> shader_context) {
    auto fill = shape.fill();
    auto local_bounds = shape.local_bounds();
    if (local_bounds.width == 0 || local_bounds.height == 0) {
        return;
    }

    Util::Size2f texture_size { fill.texture() ? fill.texture()->size().cast<float>() : Util::Size2f {} };
    auto texture_rect = fill.texture_rect();
    if (texture_rect.size().is_zero()) {
        texture_rect.width = texture_size.x();
        texture_rect.height = texture_size.y();
    }

    auto normalized_texture_coord_for_point = [&](Util::Point2f point) -> Util::Point2f {
        if (!fill.texture()) {
            return {};
        }

        Util::Point2f point_normalized_coords {
            (point.x() - local_bounds.left) / local_bounds.width,
            (point.y() - local_bounds.top) / local_bounds.height,
        };
        Util::Point2f texture_coords {
            texture_rect.left + point_normalized_coords.x() * texture_rect.width,
            texture_rect.top + point_normalized_coords.y() * texture_rect.height,
        };

        return { texture_coords.x() / texture_size.x(), texture_coords.y() / texture_size.y() };
    };

    std::vector<Gfx::Vertex> fill_vertices;
    fill_vertices.reserve(vertices.size());
    for (auto const& point : vertices) {
        fill_vertices.push_back(Gfx::Vertex {
            point,
            fill.color(),
            normalized_texture_coord_for_point(point),
        });
    }

    draw_vertices(llgl::PrimitiveType::TriangleFan, fill_vertices, fill.texture(), std::move(shader_context));
}

void Painter::draw_outline(Drawing::Shape const& shape, std::span<Util::Point2f const> vertices) {
    draw_outline(vertices, shape.outline().color(), shape.outline().thickness());
}

void Painter::draw(Drawing::Shape const& shape) {
    auto const& verts = shape.vertices();
    set_submodel(shape.transform().translate(Util::Vector3f { -shape.origin().to_vector(), 0.f }));
    if (shape.fill().is_visible()) {
        draw_fill(shape, verts, shape.shader_context());
    }
    if (shape.outline().is_visible()) {
        draw_outline(shape, verts);
    }
    set_submodel(llgl::Transform {});
}

void Painter::deprecated_draw_rectangle(Util::Rectf bounds, Gfx::RectangleDrawOptions const& options) {
    draw(Drawing::Rectangle {
        bounds,
        Drawing::Fill {}.set_color(options.fill_color).set_texture(options.texture).set_texture_rect(Util::Rectf { options.texture_rect }),
        Drawing::Outline::normal(options.outline_color, options.outline_thickness)
            .set_round_radius({
                options.border_radius_top_left,
                options.border_radius_top_right,
                options.border_radius_bottom_left,
                options.border_radius_bottom_right,
            }),
    });
}

void Painter::draw_ellipse(Util::Point2f center, Util::Size2f size, DrawOptions const& options) {
    draw(Gfx::Drawing::Ellipse {
        center, size.to_vector() / 2.f,
        Drawing::Fill {}.set_color(options.fill_color).set_texture(options.texture).set_texture_rect(Util::Rectf { options.texture_rect }),
        Drawing::Outline::normal(options.outline_color, options.outline_thickness) }
             .set_point_count(30));
}

void Painter::draw_line(std::span<Util::Point2f const> positions, LineDrawOptions const& options) {
    std::vector<Gfx::Vertex> vertices;
    for (auto const& position : positions) {
        vertices.push_back({ position, options.color, {} });
    }
    draw_vertices(llgl::PrimitiveType::LineStrip, vertices);
}

void Painter::draw_outline(std::span<Util::Point2f const> positions, Util::Color color, float thickness) {
    if (thickness == 0)
        return;

    if (positions.size() < 3) {
        return;
    }

    std::vector<Gfx::Vertex> vertices;
    for (size_t i = 0; i < positions.size() + 1; i++) {
        auto A = i == 0 ? positions.back() : positions[i - 1];
        auto B = positions[(i + 1) % positions.size()];
        auto C = positions[i % positions.size()]; // fill corner
        if (A == B || A == C || B == C) {
            continue;
        }
        auto outer_corner = Drawing::round({ A, B, C, thickness }).center;
        if (thickness > 0) {
            outer_corner = outer_corner + (C - outer_corner) * 2.f;
        }

        vertices.push_back(Gfx::Vertex { outer_corner, color, {} });
        vertices.push_back(Gfx::Vertex { C, color, {} });
    }
    draw_vertices(llgl::PrimitiveType::TriangleStrip, vertices);
}

void Painter::draw_vertices(
    llgl::PrimitiveType type, std::span<Gfx::Vertex const> vertices, llgl::Texture const* texture,
    std::optional<ShaderContext> shader_context
) {
    m_builder.add_vertices(
        vertices,
        {
            .type = type,
            .projection = m_projection,
            .view = m_view,
            .model = m_model,
            .submodel = m_submodel,
            .texture = texture,
            .shader_context = std::move(shader_context),
        }
    );
}

void Painter::apply_states() const {
    OpenGL::Enable(GL_BLEND);
    OpenGL::BlendFuncSeparate(
        static_cast<GLenum>(m_blending.src_rgb),   //
        static_cast<GLenum>(m_blending.dst_rgb),   //
        static_cast<GLenum>(m_blending.src_alpha), //
        static_cast<GLenum>(m_blending.dst_alpha)
    );
}

}
