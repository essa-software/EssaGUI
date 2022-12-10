#include "Text.hpp"

namespace Gfx {

void Text::draw(Gfx::Painter& painter) const {
    float line_y = 0;
    auto cache = m_font.cache(m_font_size);
    if (!cache)
        return;
    m_string.for_each_line([this, &painter, &line_y, cache](std::span<uint32_t const> span) {
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
            painter.draw_rectangle(
                {
                    std::floor(x_position + line_position.x()),
                    std::floor(line_position.y()),
                    static_cast<float>(std::floor(glyph.texture_rect.width)),
                    static_cast<float>(std::floor(glyph.texture_rect.height)),
                },
                text_rect);
            x_position += glyph.texture_rect.width + m_font.kerning(m_font_size, previous_codepoint, codepoint);
            previous_codepoint = codepoint;
        }
    });
}

void Text::align(GUI::Align align, Util::Rectf rect) {
    auto text_size = calculate_text_size();

    Util::Vector2f size { rect.width, rect.height };
    Util::Vector2f offset;

    switch (align) {
    case GUI::Align::Top:
        offset = Util::Vector2f(std::round(size.x() / 2 - text_size.x() / 2.f), 0);
        break;
    case GUI::Align::TopRight:
        offset = Util::Vector2f(std::round(size.x() - text_size.x()), 0);
        break;
    case GUI::Align::CenterLeft:
        offset = Util::Vector2f(0, std::round(size.y() / 2 - text_size.y() / 2.f));
        break;
    case GUI::Align::Center:
        offset = Util::Vector2f(std::round(size.x() / 2 - text_size.x() / 2.f), std::round(size.y() / 2 - text_size.y() / 2.f));
        break;
    case GUI::Align::CenterRight:
        offset = Util::Vector2f(std::round(size.x() - text_size.x()), std::round(size.y() / 2 - text_size.y() / 2.f));
        break;
    default:
        // TODO: Handle other alignments
        offset = {};
    }

    m_position = Util::Vector2f { rect.left, rect.top + m_font.ascent(m_font_size) } + offset;
}

Util::Vector2u Text::calculate_text_size() const {
    return calculate_text_size(m_string);
}

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
