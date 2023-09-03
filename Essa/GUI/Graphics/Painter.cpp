#include "Painter.hpp"

#include <Essa/GUI/Graphics/Drawing/Ellipse.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Drawing/Shape.hpp>
#include <Essa/LLGL/OpenGL/PrimitiveType.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Config.hpp>
#include <algorithm>
#include <fmt/ostream.h>
#include <tracy/Tracy.hpp>

namespace Gfx {

void Painter::render() {
    ZoneScoped;
    apply_states();
    m_builder.render(m_renderer);
}

void Painter::draw_fill(Drawing::Shape const& shape, std::vector<Util::Point2f> const& vertices) {
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
        if (texture_rect.size().is_zero()) {
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

    draw_vertices(llgl::PrimitiveType::TriangleFan, fill_vertices, fill.texture());
}

void Painter::draw_outline(Drawing::Shape const& shape, std::vector<Util::Point2f> const& vertices) {
    draw_outline(vertices, shape.outline().color(), shape.outline().thickness());
}

struct RoundingResult {
    Util::Point2f center;
    float angle_start;
    float angle_end;
    float scaled_radius;
};
struct RoundingSettings {
    Util::Point2f left;
    Util::Point2f right;
    Util::Point2f tip;
    float radius;
};

RoundingResult round(RoundingSettings settings) {
    // fmt::print("--- ROUND ---\n");
    auto& A = settings.left;
    auto& B = settings.right;
    auto& C = settings.tip;
    assert(A != B);
    assert(B != C);
    assert(A != C);
    auto& r = settings.radius;
    // fmt::print("A={} B={} C={} r={}\n", fmt::streamed(A), fmt::streamed(B), fmt::streamed(C), r);

    // 1)
    auto d_a = A - C;
    auto d_b = B - C;
    // fmt::print("DA={}, DB={}\n", fmt::streamed(d_a), fmt::streamed(d_b));

    auto tg_gamma_rec = -(d_a.x() * d_b.x() + d_a.y() * d_b.y()) / (d_a.y() * d_b.x() - d_a.x() * d_b.y());

    // 2a)
    auto cos_gamma = static_cast<float>(d_a.normalized().dot(d_b.normalized()));
    auto sin_gamma = static_cast<float>(tg_gamma_rec == 0 ? 1 : cos_gamma / tg_gamma_rec);
    // fmt::print("sin={}, cos={}\n", sin_gamma, cos_gamma);

    // 2b)
    auto tg_half_gamma = 1 / sin_gamma - tg_gamma_rec;

    // 2c)
    // fmt::print("1/tg y = {}; tg 1/2y = {}\n", tg_gamma_rec, tg_half_gamma);
    // fmt::print("r = {}\n", r);
    auto expected_a = std::abs(r / tg_half_gamma);
    auto a = expected_a;

    // Scale everything down if doesn't fit.
    a = std::min<float>(a, static_cast<float>(d_a.length()) / 2);
    a = std::min<float>(a, static_cast<float>(d_b.length()) / 2);
    r *= a / expected_a;
    // fmt::print("a ..= {} {}\n", expected_a, a);

    // 3)
    auto ca_p = d_a.with_length(a);
    auto cb_p = d_b.with_length(a);
    // fmt::print("ca = {} {}\n", fmt::streamed(ca_p), fmt::streamed(cb_p));

    // 4a)
    auto d = std::hypot(r, a);

    // 4b)
    auto center = C + (ca_p + cb_p).with_length(d);

    // 5)
    auto alpha = ((C + ca_p) - center).angle();
    auto beta = ((C + cb_p) - center).angle();
    if (beta - alpha > 180_deg) {
        alpha += 360_deg;
    }
    if (alpha - beta > 180_deg) {
        beta += 360_deg;
    }
    assert(std::abs((alpha - beta).rad()) < M_PI + 10e-6);

    // fmt::print("{} {}\n", fmt::streamed(settings.tip), fmt::streamed(center));
    return { center, static_cast<float>(alpha.rad()), static_cast<float>(beta.rad()), r };
}

static std::vector<Util::Point2f> calculate_vertices_for_rounded_shape(Drawing::Shape const& shape) {
    std::vector<Util::Point2f> vertices;

    auto round_radius_for_vertex = [&](size_t idx) {
        if (shape.point_count() == 4) {
            switch (idx) {
            case 0:
                return shape.outline().round_radius().top_left;
            case 1:
                return shape.outline().round_radius().top_right;
            case 2:
                return shape.outline().round_radius().bottom_right;
            case 3:
                return shape.outline().round_radius().bottom_left;
            default:
                ESSA_UNREACHABLE;
            }
        }

        // We don't support separate round radiuses for other shapes
        // than rectangle, for now.
        return shape.outline().round_radius().top_left;
    };

    for (size_t s = 0; s < shape.point_count(); s++) {
        auto r = round_radius_for_vertex(s);

        auto tip = shape.point(s);
        if (r == 0) {
            vertices.push_back(tip);
            continue;
        }

        auto left = shape.point(s == 0 ? shape.point_count() - 1 : s - 1);
        auto right = shape.point(s == shape.point_count() - 1 ? 0 : s + 1);

        auto rounding = round({ left, right, tip, r });

        constexpr size_t RoundingResolution = 12;
        for (size_t s = 0; s <= RoundingResolution; s++) {
            float angle = rounding.angle_start
                + (rounding.angle_end - rounding.angle_start) * static_cast<float>(s) / static_cast<float>(RoundingResolution);
            // fmt::print("{}\n", fmt::streamed(rounding.center));
            auto point = rounding.center + Util::Vector2f::create_polar(Util::Angle::radians(angle), rounding.scaled_radius);

            // Don't allow duplicates
            if (vertices.empty() || !point.is_approximately_equal(vertices.back())) {
                vertices.push_back(point);
            }
        }
    }

    // Don't allow duplicates
    if (vertices.front().is_approximately_equal(vertices.back())) {
        vertices.pop_back();
    }

    // fmt::print("--- Rounded Vertices ---\n");
    // for (auto const& v : vertices) {
    //     fmt::print("* {}\n", fmt::streamed(v));
    // }

    return vertices;
}

void Painter::draw(Drawing::Shape const& shape) {
    std::vector<Util::Point2f> vertices_for_rounded_shape = [&]() {
        if (!shape.outline().is_rounded()) {
            return shape.points().to_vector();
        }
        return calculate_vertices_for_rounded_shape(shape);
    }();

    m_builder.set_submodel(shape.transform().translate(Util::Vector3f { -shape.origin().to_vector(), 0.f }));
    if (shape.fill().is_visible()) {
        draw_fill(shape, vertices_for_rounded_shape);
    }
    if (shape.outline().is_visible()) {
        draw_outline(shape, vertices_for_rounded_shape);
    }
    m_builder.set_submodel(llgl::Transform {});
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

    std::vector<Gfx::Vertex> vertices;
    for (size_t i = 0; i < positions.size() + 1; i++) {
        auto A = i == 0 ? positions.back() : positions[i - 1];
        auto B = positions[(i + 1) % positions.size()];
        auto C = positions[i % positions.size()]; // fill corner
        if (A == B || A == C || B == C) {
            continue;
        }
        auto outer_corner = round({ A, B, C, thickness }).center;
        if (thickness > 0) {
            outer_corner = outer_corner + (C - outer_corner) * 2.f;
        }

        vertices.push_back(Gfx::Vertex { outer_corner, color, {} });
        vertices.push_back(Gfx::Vertex { C, color, {} });
    }
    draw_vertices(llgl::PrimitiveType::TriangleStrip, vertices);
}

void Painter::draw_vertices(llgl::PrimitiveType type, std::span<Gfx::Vertex const> vertices, llgl::Texture const* texture) {
    m_builder.add_vertices(type, vertices, texture);
}

void Painter::apply_states() {
    OpenGL::Enable(GL_BLEND);
    OpenGL::BlendFuncSeparate(
        static_cast<GLenum>(m_blending.src_rgb),   //
        static_cast<GLenum>(m_blending.dst_rgb),   //
        static_cast<GLenum>(m_blending.src_alpha), //
        static_cast<GLenum>(m_blending.dst_alpha)
    );
}

}
