#include "RichText.hpp"

#include <Essa/GUI/Graphics/RichText/Fragments.hpp>
#include <EssaUtil/Is.hpp>

namespace Gfx {

void RichText::append(Util::UString const& string, Util::Color const& color) {
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
        fmt::print("{}..{}\n", index, *next);
        append_fragment<RichTextFragments::Text>(string.substring(index, *next - index + 1), color);
        if (string.at(*next) == '\n') {
            append_fragment<RichTextFragments::LineBreak>();
        }
        if (next == string.size())
            break;
        index = *next + 1;
    }
}

void RichTextDrawable::draw(Gfx::Painter& painter) const {
    constexpr float LineHeight = 30;
    Util::Vector2f current_position;
    current_position.y() += LineHeight;

    for (auto const& frag : m_text.fragments()) {
        if (Util::is<RichTextFragments::LineBreak>(*frag)) {
            current_position.x() = 0;
            current_position.y() += LineHeight;
            continue;
        }
        auto wanted_size = frag->wanted_size(m_context);
        if (current_position.x() + wanted_size > m_rect.width) {
            current_position.x() = 0;
            current_position.y() += LineHeight;
        }
        frag->draw(m_context, current_position + m_rect.position(), painter);
        current_position.x() += wanted_size;
    }
}

}
