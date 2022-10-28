#include "Painter.hpp"

#include <LLGL/OpenGL/PrimitiveType.hpp>
#include <LLGL/OpenGL/Transform.hpp>

namespace Gfx {

constexpr size_t RoundedEdgeResolution = 10;
constexpr size_t RoundedRectanglePointCount = RoundedEdgeResolution * 4;

static Util::Vector2f get_rounded_rectangle_vertex(std::size_t index, Util::Vector2f size, Gfx::RectangleDrawOptions const& options) {
    size_t side = index / RoundedEdgeResolution;

    float subangle = (float)(index % RoundedEdgeResolution) / (RoundedEdgeResolution - 1) * M_PI_2;
    float angle = subangle + side * M_PI_2;

    float radius_top_left = std::min(std::min(options.border_radius_top_left, size.x() / 2), size.y() / 2);
    float radius_top_right = std::min(std::min(options.border_radius_top_right, size.x() / 2), size.y() / 2);
    float radius_bottom_left = std::min(std::min(options.border_radius_bottom_left, size.x() / 2), size.y() / 2);
    float radius_bottom_right = std::min(std::min(options.border_radius_bottom_right, size.x() / 2), size.y() / 2);

    Util::Vector2f offset;
    Util::Vector2f base;
    switch (side) {
    case 0:
        base = { size.x() - radius_bottom_right, size.y() - radius_bottom_right };
        offset = { std::cos(angle) * radius_bottom_right, std::sin(angle) * radius_bottom_right };
        break;
    case 1:
        base = { radius_bottom_left, size.y() - radius_bottom_left };
        offset = { std::cos(angle) * radius_bottom_left, std::sin(angle) * radius_bottom_left };
        break;
    case 2:
        base = { radius_top_left, radius_top_left };
        offset = { std::cos(angle) * radius_top_left, std::sin(angle) * radius_top_left };
        break;
    case 3:
        base = { size.x() - radius_top_right, radius_top_right };
        offset = { std::cos(angle) * radius_top_right, std::sin(angle) * radius_top_right };
        break;
    default:
        break;
    }
    auto result = base + offset;
    return result;
}

void Painter::draw_rectangle(Util::Rectf bounds, Gfx::RectangleDrawOptions const& options) {
    if (options.border_radius_bottom_left == 0 && options.border_radius_bottom_right == 0 && options.border_radius_top_left == 0 && options.border_radius_top_right == 0) {
        Util::Vector2f normalized_texture_rect_position = options.texture
            ? Util::Vector2f { static_cast<float>(options.texture_rect.position().x()) / options.texture->size().x(),
                  static_cast<float>(options.texture_rect.position().y()) / options.texture->size().y() }
            : Util::Vector2f {};

        Util::Vector2f normalized_texture_rect_size = options.texture
            ? Util::Vector2f { static_cast<float>(options.texture_rect.size().x()) / options.texture->size().x(),
                  static_cast<float>(options.texture_rect.size().y()) / options.texture->size().y() }
            : Util::Vector2f {};

        if (normalized_texture_rect_position == Util::Vector2f {} && normalized_texture_rect_size == Util::Vector2f {})
            normalized_texture_rect_size = { 1, 1 };

        std::array<Util::Vector2f, 4> outline_positions;

        outline_positions[0] = { bounds.left, bounds.top };
        outline_positions[1] = { bounds.left + bounds.width, bounds.top };
        outline_positions[2] = { bounds.left + bounds.width, bounds.top + bounds.height };
        outline_positions[3] = { bounds.left, bounds.top + bounds.height };

        m_builder.add_rectangle(bounds, GUIBuilder::RectangleDrawOptions {
                                            .texture = options.texture,
                                            .texture_rect = { normalized_texture_rect_position, normalized_texture_rect_size },
                                            .fill_color = options.fill_color,
                                        });
        draw_outline(outline_positions, options.outline_color, options.outline_thickness);
        return;
    }

    std::array<Gfx::Vertex, RoundedRectanglePointCount> vertices;
    std::array<Util::Vector2f, RoundedRectanglePointCount> outline_positions;
    for (size_t s = 0; s < RoundedRectanglePointCount; s++) {
        Util::Vector2f vertex { get_rounded_rectangle_vertex(s, { bounds.width, bounds.height }, options) + Util::Vector2f(bounds.left, bounds.top) };
        vertices[s] = Gfx::Vertex {
            vertex,
            options.fill_color,
            { vertex.x() / bounds.width, vertex.y() / bounds.height }
        };
        outline_positions[s] = vertex;
    }
    draw_vertices(llgl::PrimitiveType::TriangleFan, vertices, options.texture);
    draw_outline(outline_positions, options.outline_color, options.outline_thickness);
}

void Painter::draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& options) {
    constexpr int VertexCount = 30;

    std::array<Gfx::Vertex, VertexCount> vertices;
    std::array<Util::Vector2f, VertexCount> outline_positions;
    for (size_t s = 0; s < VertexCount; s++) {
        float angle = 6.28 * s / VertexCount;
        Util::Vector2f vpos { size.x() / 2 * std::sin(angle), size.y() / 2 * std::cos(angle) };
        vertices[s] = Gfx::Vertex {
            Util::Vector2f(vpos + center),
            options.fill_color,
            {}
        };
        outline_positions[s] = vertices[s].position();
    }
    draw_vertices(llgl::PrimitiveType::TriangleFan, vertices);
    draw_outline(outline_positions, options.outline_color, options.outline_thickness);
}

void Painter::draw_line(std::span<Util::Vector2f const> positions, LineDrawOptions const& options) {
    std::vector<Gfx::Vertex> vertices;
    for (auto const& position : positions) {
        vertices.push_back({ position, options.color, {} });
    }
    draw_vertices(llgl::PrimitiveType::LineStrip, vertices);
}

void Painter::draw_outline(std::span<Util::Vector2f const> positions, Util::Color color, float thickness) {
    if (thickness == 0)
        return;

    std::vector<Gfx::Vertex> vertices;
    for (size_t i = 0; i < positions.size() + 1; i++) {
        // See docs/outline.xcf for proof
        auto A = i == 0 ? positions.back() : positions[i - 1];
        auto B = positions[(i + 1) % positions.size()];
        auto C = positions[i % positions.size()]; // fill corner
        auto BC = C - B;
        auto AC = C - A;
        auto cos_edge_angle = BC.dot(AC);
        auto e = thickness * std::sqrt(1 - cos_edge_angle * cos_edge_angle);
        auto A2 = BC.normalized() * e;
        auto B2 = AC.normalized() * e;
        auto CD = A2 + B2;
        auto corner = C + CD;

        vertices.push_back(Gfx::Vertex { corner, color, {} });
        vertices.push_back(Gfx::Vertex { C, color, {} });
    }
    draw_vertices(llgl::PrimitiveType::TriangleStrip, vertices);
}

void Painter::draw_vertices(llgl::PrimitiveType type, std::span<Gfx::Vertex const> vertices, llgl::Texture const* texture) {
    m_builder.add_vertices(type, vertices, texture);
}

void Painter::apply_states() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

}
