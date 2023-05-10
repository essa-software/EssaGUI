#include "Console.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/EMLError.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Text.hpp>

#include <sstream>

namespace GUI {

constexpr float LINE_SPACING = 20;

void Console::append_line(LogLine line, ScrollToEnd scroll_to_end) {
    m_lines.push_back(std::move(line));
    if (scroll_to_end == ScrollToEnd::Yes) {
        this->scroll_to_end();
    }
}

void Console::append_content(LogLine content, ScrollToEnd scroll_to_end) {
    content.text.for_each_line([&](std::span<uint32_t const> span) {
        append_line({ .color = content.color, .text = Util::UString { span } },
            ScrollToEnd::No);
    });
    if (scroll_to_end == ScrollToEnd::Yes) {
        this->scroll_to_end();
    }
}

void Console::scroll_to_end() {
    scroll_to_bottom();
    set_scroll_x(0);
}

void Console::clear() { m_lines.clear(); }

void Console::draw(Gfx::Painter& painter) const {
    size_t s = 0;
    for (auto& line : m_lines) {
        auto position
            = Util::Cs::Point2i { 5, s * LINE_SPACING + 19 } + scroll_offset();
        if (position.y() > raw_size().y() + LINE_SPACING || position.y() < 0) {
            s++;
            continue;
        }

        Gfx::Text text { line.text, Application::the().fixed_width_font() };
        text.set_fill_color(line.color);
        text.set_font_size(theme().label_font_size);
        text.set_position(position.cast<float>().to_deprecated_vector());
        text.draw(painter);
        s++;
    }

    ScrollableWidget::draw_scrollbar(painter);
}

Util::Cs::Size2i Console::content_size() const {
    float width = 0;
    auto character_size
        = Application::the().fixed_width_font().calculate_text_size(
            "x", theme().label_font_size);
    for (auto& line : m_lines) {
        float line_width = line.text.size() * character_size.x();
        if (line_width > width) {
            width = line_width;
        }
    }
    return { width + 10, m_lines.size() * LINE_SPACING + 10 };
}
EML::EMLErrorOr<void> Console::load_from_eml_object(
    EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    for (auto const& line : object.properties) {
        if (line.second.name != "line")
            continue;

        auto arr = TRY(line.second.value.to_array());

        if (arr.values().size() != 2) {
            return EML::EMLError { "Incorrect array params" };
        }

        auto text = TRY(arr.values()[0].to_string());
        auto color = TRY(arr.values()[1].to_color());

        LogLine log;
        log.color = color;
        log.text = text;

        append_line(log);
    }

    return {};
}

EML_REGISTER_CLASS(Console)

}
