#include "SFMLWindow.hpp"

#include <EssaGUI/gfx/DefaultGUIShader.hpp>

#include <SFML/Graphics/Glsl.hpp>
#include <iostream>

namespace GUI {

static bool s_glewInitialized = false;

SFMLWindow::SFMLWindow(sf::VideoMode vm, sf::String title, unsigned style, sf::ContextSettings cs)
    : Window(vm, std::move(title), style, cs) {

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

void SFMLWindow::clear(sf::Color color) {
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
    glVertexAttribPointer(0, 3, GL_DOUBLE, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glVertexAttribPointer(2, 3, GL_DOUBLE, false, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
    glDrawArrays(mode, 0, vertices.size());
}

void SFMLWindow::draw_indexed_vertices(GLenum mode, std::span<Vertex const> vertices, std::span<unsigned const> indices) {
    apply_states();
    ensure_vao_vbo();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_DOUBLE, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glVertexAttribPointer(2, 3, GL_DOUBLE, false, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
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
    shader.setUniform("modelMatrix", sf::Glsl::Mat4((float*)model_matrix.data));
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

static sf::Vector2f get_rounded_rectangle_vertex(std::size_t index, sf::Vector2f size, RectangleDrawOptions const& options) {
    size_t side = index / RoundedEdgeResolution;

    float subangle = (float)(index % RoundedEdgeResolution) / (RoundedEdgeResolution - 1) * M_PI_2;
    float angle = subangle + side * M_PI_2;

    float radius_top_left = std::min(std::min(options.border_radius_top_left, size.x / 2), size.y / 2);
    float radius_top_right = std::min(std::min(options.border_radius_top_right, size.x / 2), size.y / 2);
    float radius_bottom_left = std::min(std::min(options.border_radius_bottom_left, size.x / 2), size.y / 2);
    float radius_bottom_right = std::min(std::min(options.border_radius_bottom_right, size.x / 2), size.y / 2);

    sf::Vector2f offset;
    sf::Vector2f base;
    switch (side) {
    case 0:
        base = { size.x - radius_bottom_right, size.y - radius_bottom_right };
        offset = { std::cos(angle) * radius_bottom_right, std::sin(angle) * radius_bottom_right };
        break;
    case 1:
        base = { radius_bottom_left, size.y - radius_bottom_left };
        offset = { std::cos(angle) * radius_bottom_left, std::sin(angle) * radius_bottom_left };
        break;
    case 2:
        base = { radius_top_left, radius_top_left };
        offset = { std::cos(angle) * radius_top_left, std::sin(angle) * radius_top_left };
        break;
    case 3:
        base = { size.x - radius_top_right, radius_top_right };
        offset = { std::cos(angle) * radius_top_right, std::sin(angle) * radius_top_right };
        break;
    default:
        break;
    }
    auto result = base + offset;
    return result;
}

void SFMLWindow::draw_rectangle(sf::FloatRect bounds, RectangleDrawOptions const& options) {
    if (options.border_radius_bottom_left == 0 && options.border_radius_bottom_right == 0 && options.border_radius_top_left == 0 && options.border_radius_top_right == 0) {
        std::array<Vertex, 4> vertices;
        vertices[0] = Vertex { .position = { bounds.left, bounds.top }, .color = options.fill_color };
        vertices[1] = Vertex { .position = { bounds.left + bounds.width, bounds.top }, .color = options.fill_color };
        vertices[2] = Vertex { .position = { bounds.left, bounds.top + bounds.height }, .color = options.fill_color };
        vertices[3] = Vertex { .position = { bounds.left + bounds.width, bounds.top + bounds.height }, .color = options.fill_color };
        draw_vertices(GL_TRIANGLE_STRIP, vertices);
        return;
    }

    std::array<Vertex, RoundedRectanglePointCount> vertices;
    for (size_t s = 0; s < RoundedRectanglePointCount; s++) {
        vertices[s] = Vertex {
            .position = Vector3(get_rounded_rectangle_vertex(s, { bounds.width, bounds.height }, options) + sf::Vector2f(bounds.left, bounds.top)),
            .color = options.fill_color
        };
    }
    draw_vertices(GL_TRIANGLE_FAN, vertices);
}

void SFMLWindow::draw_text(sf::String const& text, sf::Font const& font, sf::Vector2f position, TextDrawOptions const& options) {
    sf::Vector2f current_pos = position;
    std::vector<Vertex> vertices;

    RectangleDrawOptions debug_rect;
    debug_rect.fill_color = sf::Color::Red;

    auto texture = &font.getTexture(options.font_size);
    for (auto codepoint : text) {
        if (codepoint == '\n') {
            current_pos.x = position.x;
            current_pos.y += font.getLineSpacing(options.font_size);
            continue;
        }

        auto glyph = font.getGlyph(codepoint, options.font_size, false);
        float texture_size_y = texture->getSize().y;

        auto glyph_pos = current_pos + sf::Vector2f { glyph.bounds.left, glyph.bounds.top };
        sf::Vector2f glyph_tex_pos { static_cast<float>(glyph.textureRect.left), static_cast<float>(glyph.textureRect.top) };
        glyph_tex_pos /= texture_size_y;
        sf::Vector2f glyph_tex_size { static_cast<float>(glyph.textureRect.width), static_cast<float>(glyph.textureRect.height) };
        glyph_tex_size /= texture_size_y;

        // draw_rectangle({ glyph_pos, { glyph.bounds.width, glyph.bounds.height } }, debug_rect);

        vertices.push_back(Vertex {
            .position = { glyph_pos.x, glyph_pos.y },
            .color = options.fill_color,
            .tex_coords = { glyph_tex_pos.x, glyph_tex_pos.y } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x + glyph.bounds.width, glyph_pos.y },
            .color = options.fill_color,
            .tex_coords = { glyph_tex_pos.x + glyph_tex_size.x, glyph_tex_pos.y } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x, glyph_pos.y + glyph.bounds.height },
            .color = options.fill_color,
            .tex_coords = { glyph_tex_pos.x, glyph_tex_pos.y + glyph_tex_size.y } });

        vertices.push_back(Vertex {
            .position = { glyph_pos.x + glyph.bounds.width, glyph_pos.y },
            .color = options.fill_color,
            .tex_coords = { glyph_tex_pos.x + glyph_tex_size.x, glyph_tex_pos.y } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x, glyph_pos.y + glyph.bounds.height },
            .color = options.fill_color,
            .tex_coords = { glyph_tex_pos.x, glyph_tex_pos.y + glyph_tex_size.y } });
        vertices.push_back(Vertex {
            .position = { glyph_pos.x + glyph.bounds.width, glyph_pos.y + glyph.bounds.height },
            .color = options.fill_color,
            .tex_coords = { glyph_tex_pos.x + glyph_tex_size.x, glyph_tex_pos.y + glyph_tex_size.y } });

        current_pos.x += glyph.advance;
    }
    set_texture(texture);
    draw_vertices(GL_TRIANGLES, vertices);
    set_texture(nullptr);

    // auto size = calculate_text_size(text, font, options);
    // draw_rectangle({ position, { size.x, 1 } }, debug_rect);
    // draw_rectangle({ position, { 1, -size.y } }, debug_rect);
}

void SFMLWindow::draw_text_aligned_in_rect(sf::String const& text, sf::FloatRect rect, sf::Font const& font, TextDrawOptions const& options) {
    auto text_size = calculate_text_size(text, font, options);
    // std::cout << text_size.x << "," << text_size.y << std::endl;

    sf::Vector2f size { rect.width, rect.height };
    sf::Vector2f offset;

    // RectangleDrawOptions debug_rect;
    // debug_rect.fill_color = sf::Color::Green;
    // draw_rectangle(rect, debug_rect);

    switch (options.text_align) {
    case Align::CenterLeft:
        offset = sf::Vector2f(0, std::round(size.y / 2 - text_size.y / 2));
        break;
    case Align::Center:
        offset = sf::Vector2f(std::round(size.x / 2 - text_size.x / 2), std::round(size.y / 2 - text_size.y / 2));
        break;
    default:
        // TODO: Handle other alignments
        offset = {};
    }
    draw_text(text, font, sf::Vector2f { rect.left, rect.top + font.getGlyph('x', options.font_size, false).bounds.height } + offset, options);
}

void SFMLWindow::draw_ellipse(sf::Vector2f center, sf::Vector2f size, DrawOptions const& options) {
    constexpr int VertexCount = 30;

    std::array<Vertex, VertexCount> vertices;
    for (size_t s = 0; s < VertexCount; s++) {
        float angle = 6.28 * s / VertexCount;
        sf::Vector2f vpos { size.x / 2 * std::sin(angle), size.y / 2 * std::cos(angle) };
        vertices[s] = Vertex {
            .position = Vector3(vpos + center),
            .color = options.fill_color
        };
    }
    draw_vertices(GL_TRIANGLE_FAN, vertices);
}

sf::Vector2f SFMLWindow::calculate_text_size(sf::String const& text, sf::Font const& font, TextDrawOptions const& options) {
    sf::Vector2f total_size;
    total_size.y = font.getGlyph('x', options.font_size, false).bounds.height;
    float line_size_x = 0;
    for (auto codepoint : text) {
        if (codepoint == '\n') {
            total_size.y += font.getLineSpacing(options.font_size);
            if (line_size_x > total_size.x) {
                total_size.x = line_size_x;
            }
            line_size_x = 0;
            continue;
        }
        auto glyph = font.getGlyph(codepoint, options.font_size, false);
        line_size_x += glyph.advance;
    }
    if (line_size_x > total_size.x)
        total_size.x = line_size_x;
    return total_size;
}

}