#include "Text.hpp"

namespace Gfx {

void Text::generate_vertices() const {
    std::vector<Gfx::Vertex> vertices;

    float line_y = 0;
    auto* cache = m_font.cache(m_font_size);
    if (!cache)
        return;
    m_string.for_each_line([this, &vertices, &line_y, cache](std::span<uint32_t const> span) {
        auto line_position = m_position;
        line_position.y() -= m_font.ascent(m_font_size);
        line_position.y() += line_y;
        line_y += m_font.line_height(m_font_size);

        RectangleDrawOptions text_rect;
        text_rect.texture = &cache->atlas();
        text_rect.fill_color = m_fill_color;

        int x_position = 0;
        uint32_t previous_codepoint = 0;
        for (auto codepoint : span) {
            auto glyph = cache->ensure_glyph(m_font, codepoint);
            text_rect.texture_rect = glyph.texture_rect;
            // TODO: Take (better) advantage of GUIBuilder
            auto start = Util::Cs::Point2f { std::floor(x_position + line_position.x()), std::floor(line_position.y()) };
            auto end = start
                + Util::Cs::Vector2f {
                      static_cast<float>(std::floor(glyph.texture_rect.width)),
                      static_cast<float>(std::floor(glyph.texture_rect.height)),
                  };

            auto normalized_texture_rect
                = glyph.texture_rect.cast<float>().componentwise_divide(cache->atlas().size().cast<float>());
            auto tex_start = normalized_texture_rect.position();
            auto tex_end = normalized_texture_rect.bottom_right();

            vertices.push_back(Gfx::Vertex({ start.x(), start.y() }, Util::Colors::White, { tex_start.x(), tex_start.y() }));
            vertices.push_back(Gfx::Vertex({ end.x(), start.y() }, Util::Colors::White, { tex_end.x(), tex_start.y() }));
            vertices.push_back(Gfx::Vertex({ end.x(), end.y() }, Util::Colors::White, { tex_end.x(), tex_end.y() }));

            vertices.push_back(Gfx::Vertex({ end.x(), end.y() }, Util::Colors::White, { tex_end.x(), tex_end.y() }));
            vertices.push_back(Gfx::Vertex({ start.x(), end.y() }, Util::Colors::White, { tex_start.x(), tex_end.y() }));
            vertices.push_back(Gfx::Vertex({ start.x(), start.y() }, Util::Colors::White, { tex_start.x(), tex_start.y() }));

            x_position += glyph.texture_rect.width + m_font.kerning(m_font_size, previous_codepoint, codepoint);
            previous_codepoint = codepoint;
        }
    });

    m_vertices = std::move(vertices);
}

void Text::draw(Gfx::Painter& painter) const {
    if (!m_vertices) {
        generate_vertices();
    }
    painter.draw_vertices(llgl::PrimitiveType::Triangles, std::span(*m_vertices), &m_font.cache(m_font_size)->atlas());
}

void Text::set_font_size(uint32_t f) {
    m_font_size = f;
    m_vertices = {};
}

void Text::set_string(Util::UString s) {
    m_string = std::move(s);
    m_vertices = {};
}

void Text::set_fill_color(Util::Color c) {
    m_fill_color = c;
    m_vertices = {};
}

void Text::set_position(Util::Vector2f position) {
    m_position = position;
    m_vertices = {};
}

void Text::align(GUI::Align align, Util::Rectf rect) {
    m_vertices = {};

    auto text_size = calculate_text_size();

    Util::Vector2f size { rect.width, rect.height };
    Util::Vector2f offset;

    switch (align) {
    case GUI::Align::TopLeft:
        offset = { 0, 0 };
        break;
    case GUI::Align::Top:
        offset = { std::round(size.x() / 2 - text_size.x() / 2.f), 0 };
        break;
    case GUI::Align::TopRight:
        offset = { std::round(size.x() - text_size.x()), 0 };
        break;
    case GUI::Align::CenterLeft:
        offset = { 0, std::round(size.y() / 2 - text_size.y() / 2.f) };
        break;
    case GUI::Align::Center:
        offset = { std::round(size.x() / 2 - text_size.x() / 2.f), std::round(size.y() / 2 - text_size.y() / 2.f) };
        break;
    case GUI::Align::CenterRight:
        offset = { std::round(size.x() - text_size.x()), std::round(size.y() / 2 - text_size.y() / 2.f) };
        break;
    case GUI::Align::BottomLeft:
        offset = { 0, std::round(size.y() - text_size.y()) };
        break;
    case GUI::Align::Bottom:
        offset = { std::round(size.x() / 2 - text_size.x() / 2.f), std::round(size.y() - text_size.y()) };
        break;
    case GUI::Align::BottomRight:
        offset = { std::round(size.x() - text_size.x()), std::round(size.y() - text_size.y()) };
        break;
    }

    m_position = Util::Vector2f { rect.left, rect.top + m_font.ascent(m_font_size) } + offset;
}

Util::Vector2u Text::calculate_text_size() const { return calculate_text_size(m_string); }

Util::Vector2u Text::calculate_text_size(Util::UString const& string) const {
    Util::Vector2u text_size;
    string.for_each_line([&text_size, this](std::span<uint32_t const> span) {
        auto text = Util::UString { span };
        auto line_size = m_font.calculate_text_size(text, m_font_size);
        text_size.x() = std::max(text_size.x(), line_size.x());
        if (text_size.y() == 0)
            text_size.y() = line_size.y();
        else
            text_size.y() += m_font.line_height(m_font_size);
    });
    return text_size;
}

float Text::find_character_position(size_t index) const {
    // FIXME: Make this work for multiline strings if it would be useful
    return calculate_text_size(m_string.substring(0, index)).x();
}

}
