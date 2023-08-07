#include "RichText.hpp"
#include "Essa/GUI/Graphics/RichText/Context.hpp"

#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/Graphics/Drawing/Fill.hpp>
#include <Essa/GUI/Graphics/Drawing/Outline.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/RichText/Fragments.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/Is.hpp>

namespace Gfx {

DBG_DECLARE(Gfx_DrawRichTextBoxes);

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

RichText RichText::parse(Util::UString const& input) {
    RichText text;
    // TODO: Write a parser.
    text.append(input);
    return text;
}

Util::Size2u RichText::required_size(RichTextContext const& context) const {
    Util::Size2u size;
    auto const line_height = context.default_font.line_height(context.font_size);
    size.set_y(line_height);
    unsigned current_width = 0;
    for (auto const& frag : fragments()) {
        if (Util::is<Gfx::RichTextFragments::LineBreak>(*frag)) {
            size += Util::Vector2u(0, line_height);
            if (current_width > size.x()) {
                size.set_x(current_width);
            }
            current_width = 0;
        }
        else {
            current_width += static_cast<unsigned>(frag->wanted_size(context));
        }
    }
    if (current_width > size.x()) {
        size.set_x(current_width);
    }
    return size;
}

RichText& RichText::append(Util::UString const& string) {
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
        append_fragment<RichTextFragments::Text>(string.substring(index, *next - index + 1));
        if (string.at(*next) == '\n') {
            append_fragment<RichTextFragments::LineBreak>();
        }
        if (next == string.size())
            break;
        index = *next + 1;
    }
    return *this;
}

RichText& RichText::append_image(llgl::Texture const& texture) { return append_fragment<Gfx::RichTextFragments::Image>(texture); }

void RichTextDrawable::draw(Gfx::Painter& painter) const {
    float const line_height = static_cast<float>(m_context.default_font.line_height(m_context.font_size));

    using namespace Gfx::Drawing;
    if (DBG_ENABLED(Gfx_DrawRichTextBoxes)) {
        painter.draw(Rectangle(m_rect, Fill::none(), Outline::normal(Util::Colors::Cyan, 1)));
    }

    // 1. Calculate how many lines we need and how wide they are.
    struct Line {
        std::vector<RichTextFragments::Base*> fragments;
        float width = 0;
    };

    std::vector<Line> lines;
    {
        Line line;
        for (auto const& frag : m_text.fragments()) {
            if (Util::is<RichTextFragments::LineBreak>(*frag)) {
                lines.push_back(std::move(line));
                line = {};
                continue;
            }
            auto wanted_size = frag->wanted_size(m_context);
            if (line.width + wanted_size > m_rect.width) {
                lines.push_back(std::move(line));
                line = {};
            }
            line.fragments.push_back(frag.get());
            line.width += wanted_size;
        }
        lines.push_back(std::move(line));
    }

    // 2. Render lines, aligning them properly.
    {
        size_t current_line = 0;

        auto get_line_starting_x = [&]() -> float {
            assert(current_line < lines.size());
            switch (m_context.text_alignment) {
            case GUI::Align::TopLeft:
            case GUI::Align::CenterLeft:
            case GUI::Align::BottomLeft:
                return m_rect.left;
            case GUI::Align::Top:
            case GUI::Align::Center:
            case GUI::Align::Bottom:
                return m_rect.center().x() - lines[current_line].width / 2;
            case GUI::Align::TopRight:
            case GUI::Align::CenterRight:
            case GUI::Align::BottomRight:
                return m_rect.left + m_rect.width - lines[current_line].width;
            }
            ESSA_UNREACHABLE;
        };

        float y = [&]() -> float {
            switch (m_context.text_alignment) {
            case GUI::Align::TopLeft:
            case GUI::Align::Top:
            case GUI::Align::TopRight:
                return m_rect.top;
            case GUI::Align::CenterLeft:
            case GUI::Align::Center:
            case GUI::Align::CenterRight:
                return m_rect.top + m_rect.height / 2.f - static_cast<float>(lines.size()) * line_height / 2.f;
            case GUI::Align::BottomLeft:
            case GUI::Align::Bottom:
            case GUI::Align::BottomRight:
                return m_rect.top + m_rect.height - static_cast<float>(lines.size()) * line_height;
            }
            ESSA_UNREACHABLE;
        }();

        for (auto const& line : lines) {
            float x = get_line_starting_x();
            for (auto const& frag : line.fragments) {
                Util::Point2f position { x, y };
                float wanted_size = frag->wanted_size(m_context);
                x += wanted_size;

                Util::Size2f size { wanted_size, line_height };

                if (DBG_ENABLED(Gfx_DrawRichTextBoxes)) {
                    painter.draw(Rectangle(Util::Rectf(position, size), Fill::none(), Outline::normal(Util::Colors::Green, 1)));
                }

                frag->draw(m_context, position, painter);
            }
            y += line_height;
            current_line++;
        }
    }
}

}
