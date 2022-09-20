#include "Console.hpp"

#include "Application.hpp"
#include <EssaGUI/eml/EMLError.hpp>
#include <EssaGUI/eml/Loader.hpp>

#include <sstream>

namespace GUI {

constexpr float LINE_SPACING = 20;

void Console::append_line(LogLine line) {
    m_lines.push_back(line);
    scroll_to_bottom();
}

void Console::append_content(LogLine content) {
    content.text.for_each_line([&](std::span<uint32_t const> span) {
        append_line({ .color = content.color, .text = Util::UString { span } });
    });
}

void Console::clear() {
    m_lines.clear();
}

void Console::draw(GUI::Window& window) const {
    size_t s = 0;
    for (auto& line : m_lines) {
        auto position = Util::Vector2f { 5, s * LINE_SPACING + 19 } + scroll_offset();
        if (position.y() > raw_size().y() + LINE_SPACING || position.y() < 0) {
            s++;
            continue;
        }

        TextDrawOptions options;
        options.fill_color = line.color;
        options.font_size = theme().label_font_size;
        window.draw_text(line.text, Application::the().fixed_width_font(), position, options);
        s++;
    }

    ScrollableWidget::draw_scrollbar(window);
}

Util::Vector2f Console::content_size() const {
    return { 0, m_lines.size() * LINE_SPACING + 10 };
}
EML::EMLErrorOr<void> Console::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
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
