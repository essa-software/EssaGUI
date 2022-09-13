#include "Window.hpp"

#include <EssaGUI/gfx/DefaultGUIShader.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Rect.hpp>
#include <LLGL/OpenGL/Error.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace GUI {

// FIXME: WTF this cast
Window::Window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& settings)
    : llgl::Window { size, title, settings }
    , m_projection { llgl::Projection::ortho({ Util::Rectd { {}, Util::Vector2d { size } } }, { 0, 0, size.x(), size.y() }) } {
    llgl::opengl::enable_debug_output();
}

void Window::clear(Util::Color color) {
    set_active();
    renderer().clear(color);
}

void Window::draw_vertices(llgl::opengl::PrimitiveType type, std::span<llgl::Vertex const> vertices) {
    apply_states();
    m_temporary_vao.load_vertexes(vertices, llgl::opengl::VAO::Usage::DynamicDraw);
    renderer().draw_vao(m_temporary_vao, type, draw_state());
}

void Window::draw_indexed_vertices(llgl::opengl::PrimitiveType type, std::span<llgl::Vertex const> vertices, std::span<unsigned const> indices) {
    apply_states();
    m_temporary_vao.load_vertexes(vertices, indices, llgl::opengl::VAO::Usage::DynamicDraw);
    renderer().draw_vao(m_temporary_vao, type, draw_state());
}

void Window::apply_states() {
    set_active();
    renderer().apply_projection(m_projection);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

llgl::DrawState Window::draw_state() const {
    static Gfx::DefaultGUIShader default_shader;
    return { .shader = m_shader ?: &default_shader, .texture = m_texture, .model_matrix = m_model_matrix, .view_matrix = m_view_matrix };
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
        std::array<llgl::Vertex, 4> vertices;
        std::array<Util::Vector3f, 4> outline_positions;

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

        vertices[0] = llgl::Vertex {
            .position = { bounds.left, bounds.top, 0 },
            .color = options.fill_color,
            .tex_coord = normalized_texture_rect_position
        };
        vertices[1] = llgl::Vertex {
            .position = { bounds.left + bounds.width, bounds.top, 0 },
            .color = options.fill_color,
            .tex_coord = { normalized_texture_rect_position.x() + normalized_texture_rect_size.x(), normalized_texture_rect_position.y() }
        };
        vertices[2] = llgl::Vertex {
            .position = { bounds.left, bounds.top + bounds.height, 0 },
            .color = options.fill_color,
            .tex_coord = { normalized_texture_rect_position.x(), normalized_texture_rect_position.y() + normalized_texture_rect_size.y() }
        };
        vertices[3] = llgl::Vertex {
            .position = { bounds.left + bounds.width, bounds.top + bounds.height, 0 },
            .color = options.fill_color,
            .tex_coord = { normalized_texture_rect_position.x() + normalized_texture_rect_size.x(), normalized_texture_rect_position.y() + normalized_texture_rect_size.y() }
        };

        outline_positions[0] = vertices[0].position;
        outline_positions[1] = vertices[1].position;
        outline_positions[2] = vertices[3].position;
        outline_positions[3] = vertices[2].position;

        // TODO: Implement TextureScope
        set_texture(options.texture);
        draw_vertices(llgl::opengl::PrimitiveType::TriangleStrip, vertices);
        set_texture(nullptr);
        draw_outline(outline_positions, options.outline_color, options.outline_thickness);
        return;
    }

    std::array<llgl::Vertex, RoundedRectanglePointCount> vertices;
    std::array<Util::Vector3f, RoundedRectanglePointCount> outline_positions;
    for (size_t s = 0; s < RoundedRectanglePointCount; s++) {
        Util::Vector3f vertex { get_rounded_rectangle_vertex(s, { bounds.width, bounds.height }, options) + Util::Vector2f(bounds.left, bounds.top), 0 };
        vertices[s] = llgl::Vertex {
            .position = vertex,
            .color = options.fill_color,
            .tex_coord = { vertex.x() / bounds.width, vertex.y() / bounds.height }
        };
        outline_positions[s] = vertex;
    }
    // TODO: Implement TextureScope
    set_texture(options.texture);
    draw_vertices(llgl::opengl::PrimitiveType::TriangleFan, vertices);
    set_texture(nullptr);
    draw_outline(outline_positions, options.outline_color, options.outline_thickness);
}

void Window::draw_text(Util::UString const& text, llgl::TTFFont const& font, Util::Vector2f position, TextDrawOptions const& options) {
    float line_y = 0;
    auto cache = font.cache(options.font_size);
    if (!cache)
        return;
    text.for_each_line([&font, &options, position, &line_y, cache, this](std::span<uint32_t const> span) {
        auto line_position = position;
        line_position.y() -= font.ascent(options.font_size);
        line_position.y() += line_y;
        line_y += font.line_height(options.font_size);

        RectangleDrawOptions text_rect;
        text_rect.texture = &cache->atlas();
        text_rect.fill_color = options.fill_color;

        float x_position = 0;
        uint32_t previous_codepoint = 0;
        for (auto codepoint : span) {
            auto glyph = cache->ensure_glyph(font, codepoint);
            text_rect.texture_rect = glyph.texture_rect;
            draw_rectangle({ x_position + line_position.x(), line_position.y(), static_cast<float>(glyph.texture_rect.width), static_cast<float>(glyph.texture_rect.height) }, text_rect);
            x_position += glyph.texture_rect.width + font.kerning(options.font_size, previous_codepoint, codepoint);
            previous_codepoint = codepoint;
        }
    });
}

void Window::draw_text_aligned_in_rect(Util::UString const& text, Util::Rectf rect, llgl::TTFFont const& font, TextDrawOptions const& options) {
    auto text_size = calculate_text_size(text, font, options);

    Util::Vector2f size { rect.width, rect.height };
    Util::Vector2f offset;

    switch (options.text_align) {
    case Align::Top:
        offset = Util::Vector2f(std::round(size.x() / 2 - text_size.x() / 2.f), 0);
        break;
    case Align::TopRight:
        offset = Util::Vector2f(std::round(size.x() - text_size.x()), 0);
        break;
    case Align::CenterLeft:
        offset = Util::Vector2f(0, std::round(size.y() / 2 - text_size.y() / 2.f));
        break;
    case Align::Center:
        offset = Util::Vector2f(std::round(size.x() / 2 - text_size.x() / 2.f), std::round(size.y() / 2 - text_size.y() / 2.f));
        break;
    case Align::CenterRight:
        offset = Util::Vector2f(std::round(size.x() - text_size.x()), std::round(size.y() / 2 - text_size.y() / 2.f));
        break;
    default:
        // TODO: Handle other alignments
        offset = {};
    }

    // TODO:  font.getGlyph('x', options.font_size, false).bounds.height
    draw_text(text, font, Util::Vector2f { rect.left, rect.top + font.ascent(options.font_size) } + offset, options);
}

void Window::draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& options) {
    constexpr int VertexCount = 30;

    std::array<llgl::Vertex, VertexCount> vertices;
    std::array<Util::Vector3f, VertexCount> outline_positions;
    for (size_t s = 0; s < VertexCount; s++) {
        float angle = 6.28 * s / VertexCount;
        Util::Vector2f vpos { size.x() / 2 * std::sin(angle), size.y() / 2 * std::cos(angle) };
        vertices[s] = llgl::Vertex {
            .position = Util::Vector3f(vpos + center, 0),
            .color = options.fill_color
        };
        outline_positions[s] = vertices[s].position;
    }
    draw_vertices(llgl::opengl::PrimitiveType::TriangleFan, vertices);
    draw_outline(outline_positions, options.outline_color, options.outline_thickness);
}

void Window::draw_outline(std::span<Util::Vector3f const> positions, Util::Color color, float thickness) {
    if (thickness == 0)
        return;
    std::vector<llgl::Vertex> vertices;
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
        vertices.push_back(llgl::Vertex { .position = corner, .color = color });
        vertices.push_back(llgl::Vertex { .position = C, .color = color });
    }
    draw_vertices(llgl::opengl::PrimitiveType::TriangleStrip, vertices);
}

Util::Vector2u Window::calculate_text_size(Util::UString const& text, llgl::TTFFont const& font, TextDrawOptions const& options) {
    Util::Vector2u text_size;
    text.for_each_line([&font, &options, &text_size](std::span<uint32_t const> span) {
        auto text = Util::UString { span };
        auto line_size = font.calculate_text_size(text, options.font_size);
        text_size.x() = std::max(text_size.x(), line_size.x());
        if (text_size.y() == 0)
            text_size.y() = line_size.y();
        else
            text_size.y() += font.line_height(options.font_size);
    });
    return text_size;
}

float Window::find_character_position(size_t index, Util::UString const& string, llgl::TTFFont const& font, TextDrawOptions const& options) {
    // FIXME: Make this work for multiline strings
    return calculate_text_size(string.substring(0, index), font, options).x();
}

}
