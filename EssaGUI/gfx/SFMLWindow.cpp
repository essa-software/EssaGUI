#include "SFMLWindow.hpp"

#include <EssaGUI/gfx/DefaultGUIShader.hpp>
#include <EssaGUI/glwrapper/Vertex.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Rect.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <iostream>

namespace GUI {

static bool s_glewInitialized = false;

SFMLWindow::SFMLWindow(sf::VideoMode vm, Util::UString title, unsigned style, sf::ContextSettings cs)
    : Window(vm, sf::String::fromUtf32(title.begin(), title.end()), style, cs) {

    if (!s_glewInitialized) {
        s_glewInitialized = true;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to init glew :(" << std::endl;
            exit(1);
        }
    }
}

SFMLWindow::~SFMLWindow() {
    if (m_temporary_vbo != 0)
        glDeleteBuffers(1, &m_temporary_vbo);
    if (m_temporary_vao != 0)
        glDeleteVertexArrays(1, &m_temporary_vao);
}

void SFMLWindow::set_view(sf::View view) {
    m_view = view;
}

void SFMLWindow::clear(Util::Color color) {
    // std::cout << "------------CLEAR------------" << std::endl;
    glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void SFMLWindow::draw_vertices(GLenum mode, std::span<Vertex const> vertices) {
    apply_states();
    ensure_vao_vbo();

    // std::cout << "!!! Draw " << vertices.size() << " vertices with mode " << mode << std::endl;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glDrawArrays(mode, 0, vertices.size());
}

void SFMLWindow::draw_indexed_vertices(GLenum mode, std::span<Vertex const> vertices, std::span<unsigned const> indices) {
    apply_states();
    ensure_vao_vbo();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, indices.data());
}

void SFMLWindow::ensure_vao_vbo() {
    if (m_temporary_vao == 0) {
        glGenVertexArrays(1, &m_temporary_vao);
        glBindVertexArray(m_temporary_vao);
    }
    if (m_temporary_vbo == 0) {
        glGenBuffers(1, &m_temporary_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_temporary_vbo);
    }
}

void SFMLWindow::apply_states() {
    auto& shader = m_shader ? *m_shader : Gfx::default_gui_shader();
    shader.setUniform("viewMatrix", sf::Glsl::Mat4 { view().getTransform().getMatrix() });
    auto model_matrix = m_model_matrix.convert<float>();
    shader.setUniform("modelMatrix", sf::Glsl::Mat4(model_matrix.raw_data()));
    if (m_texture) {
        glEnable(GL_TEXTURE_2D);
        shader.setUniform("useTexture", true);
        shader.setUniform("texture", *m_texture);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        shader.setUniform("useTexture", false);
    }
    sf::Shader::bind(&shader);

    auto viewport = view().getViewport();
    viewport.left *= getSize().x;
    viewport.top *= getSize().y;
    viewport.width *= getSize().x;
    viewport.height *= getSize().y;
    // std::cout << viewport.left << "," << viewport.top << ":" << viewport.width << "," << viewport.height << std::endl;
    glViewport(viewport.left, getSize().y - viewport.top - viewport.height, viewport.width, viewport.height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void SFMLWindow::draw_rectangle(Util::Rectf bounds, RectangleDrawOptions const& options) {
    if (options.border_radius_bottom_left == 0 && options.border_radius_bottom_right == 0 && options.border_radius_top_left == 0 && options.border_radius_top_right == 0) {
        std::array<Vertex, 4> vertices;
        std::array<Util::Vector3f, 4> outline_positions;
        vertices[0] = Vertex { .position = { bounds.left, bounds.top, 0 }, .color = options.fill_color, .tex_coord = {} };
        vertices[1] = Vertex { .position = { bounds.left + bounds.width, bounds.top, 0 }, .color = options.fill_color, .tex_coord = { 1, 0 } };
        vertices[2] = Vertex { .position = { bounds.left, bounds.top + bounds.height, 0 }, .color = options.fill_color, .tex_coord = { 0, 1 } };
        vertices[3] = Vertex { .position = { bounds.left + bounds.width, bounds.top + bounds.height, 0 }, .color = options.fill_color, .tex_coord = { 1, 1 } };

        outline_positions[0] = vertices[0].position;
        outline_positions[1] = vertices[1].position;
        outline_positions[2] = vertices[3].position;
        outline_positions[3] = vertices[2].position;

        // TODO: Implement TextureScope
        set_texture(options.texture);
        draw_vertices(GL_TRIANGLE_STRIP, vertices);
        set_texture(nullptr);
        draw_outline(outline_positions, options.outline_color, options.outline_thickness);
        return;
    }

    std::array<Vertex, RoundedRectanglePointCount> vertices;
    std::array<Util::Vector3f, RoundedRectanglePointCount> outline_positions;
    for (size_t s = 0; s < RoundedRectanglePointCount; s++) {
        Util::Vector3f vertex { get_rounded_rectangle_vertex(s, { bounds.width, bounds.height }, options) + Util::Vector2f(bounds.left, bounds.top), 0 };
        vertices[s] = Vertex {
            .position = vertex,
            .color = options.fill_color,
            .tex_coord = { vertex.x() / bounds.width, vertex.y() / bounds.height }
        };
        outline_positions[s] = vertex;
    }
    // TODO: Implement TextureScope
    set_texture(options.texture);
    draw_vertices(GL_TRIANGLE_FAN, vertices);
    set_texture(nullptr);
    draw_outline(outline_positions, options.outline_color, options.outline_thickness);
}

void SFMLWindow::draw_text(Util::UString const& text, sf::Font const& font, Util::Vector2f position, TextDrawOptions const& options) {
    Util::Vector2f current_pos = position;
    std::vector<Vertex> vertices;

    RectangleDrawOptions debug_rect;
    debug_rect.fill_color = Util::Colors::red;

    auto texture = &font.getTexture(options.font_size);
    for (auto codepoint : text) {
        if (codepoint == '\n') {
            current_pos.x() = position.x();
            current_pos.y() += font.getLineSpacing(options.font_size);
            continue;
        }

        auto glyph = font.getGlyph(codepoint, options.font_size, false);
        float texture_size_y = texture->getSize().y;

        auto glyph_pos = current_pos + Util::Vector2f { glyph.bounds.left, glyph.bounds.top };
        Util::Vector2f glyph_tex_pos { static_cast<float>(glyph.textureRect.left), static_cast<float>(glyph.textureRect.top) };
        glyph_tex_pos /= texture_size_y;
        Util::Vector2f glyph_tex_size { static_cast<float>(glyph.textureRect.width), static_cast<float>(glyph.textureRect.height) };
        glyph_tex_size /= texture_size_y;

        // draw_rectangle({ glyph_pos, { glyph.bounds.width, glyph.bounds.height } }, debug_rect);

        vertices.push_back(Vertex {
            .position = { glyph_pos.x(), glyph_pos.y(), 0 },
            .color = options.fill_color,
            .tex_coord = { glyph_tex_pos.x(), glyph_tex_pos.y() } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x() + glyph.bounds.width, glyph_pos.y(), 0 },
            .color = options.fill_color,
            .tex_coord = { glyph_tex_pos.x() + glyph_tex_size.x(), glyph_tex_pos.y() } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x(), glyph_pos.y() + glyph.bounds.height, 0 },
            .color = options.fill_color,
            .tex_coord = { glyph_tex_pos.x(), glyph_tex_pos.y() + glyph_tex_size.y() } });

        vertices.push_back(Vertex {
            .position = { glyph_pos.x() + glyph.bounds.width, glyph_pos.y(), 0 },
            .color = options.fill_color,
            .tex_coord = { glyph_tex_pos.x() + glyph_tex_size.x(), glyph_tex_pos.y() } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x(), glyph_pos.y() + glyph.bounds.height, 0 },
            .color = options.fill_color,
            .tex_coord = { glyph_tex_pos.x(), glyph_tex_pos.y() + glyph_tex_size.y() } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x() + glyph.bounds.width, glyph_pos.y() + glyph.bounds.height, 0 },
            .color = options.fill_color,
            .tex_coord = { glyph_tex_pos.x() + glyph_tex_size.x(), glyph_tex_pos.y() + glyph_tex_size.y() } });

        current_pos.x() += glyph.advance;
    }
    set_texture(texture);
    draw_vertices(GL_TRIANGLES, vertices);
    set_texture(nullptr);

    // auto size = calculate_text_size(text, font, options);
    // draw_rectangle({ position, { size.x(), 1 } }, debug_rect);
    // draw_rectangle({ position, { 1, -size.y() } }, debug_rect);
}

void SFMLWindow::draw_text_aligned_in_rect(Util::UString const& text, Util::Rectf rect, sf::Font const& font, TextDrawOptions const& options) {
    auto text_size = calculate_text_size(text, font, options);
    // std::cout << text_size.x() << "," << text_size.y() << std::endl;

    Util::Vector2f size { rect.width, rect.height };
    Util::Vector2f offset;

    // RectangleDrawOptions debug_rect;
    // debug_rect.fill_color = Util::Color::Green;
    // draw_rectangle(rect, debug_rect);

    switch (options.text_align) {
    case Align::Top:
        offset = Util::Vector2f(std::round(size.x() / 2 - text_size.x() / 2), 0);
        break;
    case Align::TopRight:
        offset = Util::Vector2f(std::round(size.x() - text_size.x()), 0);
        break;
    case Align::CenterLeft:
        offset = Util::Vector2f(0, std::round(size.y() / 2 - text_size.y() / 2));
        break;
    case Align::Center:
        offset = Util::Vector2f(std::round(size.x() / 2 - text_size.x() / 2), std::round(size.y() / 2 - text_size.y() / 2));
        break;
    case Align::CenterRight:
        offset = Util::Vector2f(std::round(size.x() - text_size.x()), std::round(size.y() / 2 - text_size.y() / 2));
        break;
    default:
        // TODO: Handle other alignments
        offset = {};
    }
    draw_text(text, font, Util::Vector2f { rect.left, rect.top + font.getGlyph('x', options.font_size, false).bounds.height } + offset, options);
}

void SFMLWindow::draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& options) {
    constexpr int VertexCount = 30;

    std::array<Vertex, VertexCount> vertices;
    std::array<Util::Vector3f, VertexCount> outline_positions;
    for (size_t s = 0; s < VertexCount; s++) {
        float angle = 6.28 * s / VertexCount;
        Util::Vector2f vpos { size.x() / 2 * std::sin(angle), size.y() / 2 * std::cos(angle) };
        vertices[s] = Vertex {
            .position = Util::Vector3f(vpos + center, 0),
            .color = options.fill_color
        };
        outline_positions[s] = vertices[s].position;
    }
    draw_vertices(GL_TRIANGLE_FAN, vertices);
    draw_outline(outline_positions, options.outline_color, options.outline_thickness);
}

void SFMLWindow::draw_outline(std::span<Util::Vector3f const> positions, Util::Color color, float thickness) {
    if (thickness == 0)
        return;
    std::vector<Vertex> vertices;
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
        vertices.push_back(Vertex { .position = corner, .color = color });
        vertices.push_back(Vertex { .position = C, .color = color });
    }
    draw_vertices(GL_TRIANGLE_STRIP, vertices);
}

Util::Vector2f SFMLWindow::calculate_text_size(Util::UString const& text, sf::Font const& font, TextDrawOptions const& options) {
    Util::Vector2f total_size;
    total_size.y() = font.getGlyph('x', options.font_size, false).bounds.height;
    float line_size_x = 0;
    for (auto codepoint : text) {
        if (codepoint == '\n') {
            total_size.y() += font.getLineSpacing(options.font_size);
            if (line_size_x > total_size.x()) {
                total_size.x() = line_size_x;
            }
            line_size_x = 0;
            continue;
        }
        auto glyph = font.getGlyph(codepoint, options.font_size, false);
        line_size_x += glyph.advance;
    }
    if (line_size_x > total_size.x())
        total_size.x() = line_size_x;
    return total_size;
}

Util::Vector2f SFMLWindow::find_character_position(size_t index, Util::UString const& text, sf::Font const& font, TextDrawOptions const& options) {
    Util::Vector2f current_position;
    for (size_t s = 0; s < index; s++) {
        auto codepoint = text.at(s);
        if (codepoint == '\n') {
            current_position.y() += font.getLineSpacing(options.font_size);
            current_position.x() = 0;
            continue;
        }
        auto glyph = font.getGlyph(codepoint, options.font_size, false);
        current_position.x() += glyph.advance;
    }
    return current_position;
}
}
