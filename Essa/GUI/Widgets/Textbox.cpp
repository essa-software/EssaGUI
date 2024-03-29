#include "Textbox.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/CharacterType.hpp>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <string>

namespace GUI {

bool Textbox::find_decimal() const {
    for (auto const& c : content()) {
        if (c == '.')
            return true;
    }
    return false;
}

void Textbox::m_fit_in_range() {
    if (is_focused())
        return;

    auto maybe_value = content().parse<double>();
    if (maybe_value.is_error()) {
        return;
    }
    double value = maybe_value.release_value();
    if (value < m_min)
        value = m_min;
    else if (value > m_max)
        value = m_max;
    set_content_impl(Util::UString { fmt::format("{:.1f}", value) });
}

bool Textbox::can_insert_codepoint(uint32_t ch) const {
    if (content().size() >= m_limit)
        return false;

    switch (m_type) {
    case TEXT:
        return isprint(ch);
    case NUMBER:
        return isdigit(ch) || (ch == '.' && !m_has_decimal)
            || (ch == '-' && real_cursor_position().column == 0 && (content().is_empty() || content().at(0) != '-'));
    }
    return false;
}

void Textbox::on_content_change() {
    if (m_type == Type::NUMBER)
        m_fit_in_range();
}

EML::EMLErrorOr<void> Textbox::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(TextEditor::load_from_eml_object(object, loader));
    m_limit = TRY(object.get_property("limit", EML::Value(static_cast<double>(m_limit))).to_double());
    m_min = TRY(object.get_property("min_value", EML::Value(m_min)).to_double());
    m_max = TRY(object.get_property("max_value", EML::Value(m_max)).to_double());
    return {};
}

EML_REGISTER_CLASS(Textbox);

}
