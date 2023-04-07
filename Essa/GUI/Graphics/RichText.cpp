#include "RichText.hpp"

#include <Essa/GUI/Graphics/RichText/Fragments.hpp>
#include <EssaUtil/Is.hpp>

namespace Gfx {

RichText::RichText(RichText const& other) {
    for (auto const& fragment : other.fragments()) {
        m_fragments.push_back(fragment->clone());
    }
}

RichText& RichText::operator=(RichText const& other) {
    if (this == &other) {
        return *this;
    }

    m_fragments.clear();
    for (auto const& fragment : other.fragments()) {
        m_fragments.push_back(fragment->clone());
    }

    return *this;
}

RichText& RichText::append(Util::UString const& string, Util::Color const& color) {
    size_t index = 0;
    while (true) {
        if (index >= string.size()) {
            break;
        }
        auto next = string.find_one_of({ ' ', '\n' }, index);
        if (!next.has_value()) {
            next = string.size() - 1;
        }
        if (index > string.size() - 1)
            break;
        append_fragment<RichTextFragments::Text>(string.substring(index, *next - index + 1), color);
        if (string.at(*next) == '\n') {
            append_fragment<RichTextFragments::LineBreak>();
        }
        if (next == string.size())
            break;
        index = *next + 1;
    }
    return *this;
}

RichText& RichText::append_image(llgl::Texture const& texture) {
    return append_fragment<Gfx::RichTextFragments::Image>(texture);
}

void RichTextDrawable::draw(Gfx::Painter& painter) const {
    float const line_height = static_cast<float>(m_context.default_font.line_height(m_context.font_size));

    // 1. Calculate how many lines we need.
    size_t line_count = 1;
    {
        Util::Vector2f current_position;
        for (auto const& frag : m_text.fragments()) {
            if (Util::is<RichTextFragments::LineBreak>(*frag)) {
                current_position.x() = 0;
                current_position.y() += line_height;
                line_count++;
                continue;
            }
            auto wanted_size = frag->wanted_size(m_context);
            if (current_position.x() + wanted_size > m_rect.width) {
                current_position.x() = 0;
                current_position.y() += line_height;
                line_count++;
            }
            current_position.x() += wanted_size;
        }
    }

    // 2. Render lines, aligning them properly.
    {
        Util::Vector2f current_position;
        switch (m_context.text_alignment) {
        case GUI::Align::TopLeft:
        case GUI::Align::Top:
        case GUI::Align::TopRight:
            current_position.y() = 0;
            break;
        case GUI::Align::CenterLeft:
        case GUI::Align::Center:
        case GUI::Align::CenterRight:
            current_position.y() = m_rect.height / 2.f - static_cast<float>(line_count) * line_height / 2.f;
            break;
        case GUI::Align::BottomLeft:
        case GUI::Align::Bottom:
        case GUI::Align::BottomRight:
            current_position.y() = m_rect.height - static_cast<float>(line_count) * line_height;
            break;
        }
        for (auto const& frag : m_text.fragments()) {
            if (Util::is<RichTextFragments::LineBreak>(*frag)) {
                current_position.x() = 0;
                current_position.y() += line_height;
                continue;
            }
            auto wanted_size = frag->wanted_size(m_context);
            if (current_position.x() + wanted_size > m_rect.width) {
                current_position.x() = 0;
                current_position.y() += line_height;
            }
            frag->draw(m_context, (Util::Cs::Point2f::from_deprecated_vector(current_position) + m_rect.position().to_vector()).to_deprecated_vector(), painter);
            current_position.x() += wanted_size;
        }
    }
}

}
