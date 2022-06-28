#include "Textbox.hpp"

#include "Application.hpp"
#include "NotifyUser.hpp"
#include "Widget.hpp"
#include <EssaGUI/util/CharacterType.hpp>
#include <SFML/Graphics.hpp>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace GUI {

bool Textbox::find_decimal() const {
    for (const auto& c : get_content()) {
        if (c == '.')
            return true;
    }
    return false;
}

std::string Textbox::m_fix_content(std::string content) const {
    unsigned i = content.size();
    while (i > 0 && content[i - 1] == '0')
        i--;

    if (content[i - 1] == '.')
        return content.substr(0, i) + '0';
    return content.substr(0, std::min(m_limit, i));
}

void Textbox::m_fit_in_range() {
    if (is_focused())
        return;

    try {
        double val = std::stod(get_content().toAnsiString());
        std::ostringstream oss;
        oss << std::fixed;
        if (val < m_min_value)
            oss << m_min_value;
        else if (val > m_max_value)
            oss << m_max_value;
        else
            return;
        set_content(m_fix_content(oss.str()));
    } catch (...) {
        return;
    }
}

bool Textbox::can_insert_codepoint(uint32_t ch) const {
    if (get_content().getSize() >= m_limit)
        return false;

    switch (m_type) {
    case TEXT:
        return isprint(ch);
    case NUMBER:
        return isdigit(ch) || (ch == '.' && !m_has_decimal);
    }
    return false;
}

void Textbox::on_content_change() {
    if (m_type == Type::NUMBER)
        m_fit_in_range();
}

}
