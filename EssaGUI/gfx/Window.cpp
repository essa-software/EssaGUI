#include "Window.hpp"

#include <EssaGUI/gfx/DefaultGUIShader.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Rect.hpp>
#include <LLGL/OpenGL/Error.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace GUI {

Window::Window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& settings)
    : llgl::Window { size, title, settings }
    , m_projection { llgl::Projection::ortho({ Util::Rectd { {}, Util::Vector2d { size } } }, { 0, 0, size.x(), size.y() }) } {
    llgl::opengl::enable_debug_output();
}

void Window::clear(Util::Color color) {
    set_active();
    renderer().clear(color);
}

void Window::draw_vertices(llgl::PrimitiveType type, std::span<Gfx::Vertex const> vertices) {
    apply_states();
    // TODO: Dynamic draw
    m_temporary_vao.upload_vertices(vertices);
    renderer().draw_vertices(m_temporary_vao, llgl::DrawState { m_default_shader, type });
}

void Window::draw_indexed_vertices(llgl::PrimitiveType type, std::span<Gfx::Vertex const> vertices, std::span<unsigned const> indices) {
    apply_states();
    // TODO: Dynamic draw
    m_temporary_vao.upload_vertices(vertices, indices);
    renderer().draw_vertices(m_temporary_vao, llgl::DrawState { m_default_shader, type });
}

void Window::apply_states() {
    set_active();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // FIXME: Get rid of global state
    m_default_shader.set_texture(m_texture);
    m_default_shader.set_projection_matrix(projection().matrix());
    auto viewport = projection().viewport();
    llgl::set_viewport({ viewport.left, size().y() - viewport.top - viewport.height, viewport.width, viewport.height });
}

constexpr size_t RoundedEdgeResolution = 10;
constexpr size_t RoundedRectanglePointCount = RoundedEdgeResolution * 4;

static Util::Vector2f get_rounded_rectangle_vertex(std::size_t index, Util::Vector2f size, RectangleDrawOptions const& options) {
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

void Window::draw_rectangle(Util::Rectf bounds, RectangleDrawOptions const& options) {
    if (options.border_radius_bottom_left == 0 && options.border_radius_bottom_right == 0 && options.border_radius_top_left == 0 && options.border_radius_top_right == 0) {
        std::array<Gfx::Vertex, 4> vertices;
        std::array<Util::Vector2f, 4> outline_positions;

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

        vertices[0] = Gfx::Vertex {
            { bounds.left, bounds.top },
            options.fill_color,
            normalized_texture_rect_position
        };
        vertices[1] = Gfx::Vertex {
            { bounds.left + bounds.width, bounds.top },
            options.fill_color,
            { normalized_texture_rect_position.x() + normalized_texture_rect_size.x(), normalized_texture_rect_position.y() }
        };
        vertices[2] = Gfx::Vertex {
            { bounds.left, bounds.top + bounds.height },
            options.fill_color,
            { normalized_texture_rect_position.x(), normalized_texture_rect_position.y() + normalized_texture_rect_size.y() }
        };
        vertices[3] = Gfx::Vertex {
            { bounds.left + bounds.width, bounds.top + bounds.height },
            options.fill_color,
            { normalized_texture_rect_position.x() + normalized_texture_rect_size.x(), normalized_texture_rect_position.y() + normalized_texture_rect_size.y() }
        };

        outline_positions[0] = vertices[0].position();
        outline_positions[1] = vertices[1].position();
        outline_positions[2] = vertices[3].position();
        outline_positions[3] = vertices[2].position();

        // FIXME: Get rid of global state
        set_texture(options.texture);
        draw_vertices(llgl::PrimitiveType::TriangleStrip, vertices);
        set_texture(nullptr);
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
    // FIXME: Get rid of global state
    set_texture(options.texture);
    draw_vertices(llgl::PrimitiveType::TriangleFan, vertices);
    set_texture(nullptr);
    draw_outline(outline_positions, options.outline_color, options.outline_thickness);
}

void Window::draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& options) {
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

void Window::draw_outline(std::span<Util::Vector2f const> positions, Util::Color color, float thickness) {
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

}
