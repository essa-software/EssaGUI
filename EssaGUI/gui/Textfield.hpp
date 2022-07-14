#pragma once

#include "TextAlign.hpp"
#include "Widget.hpp"

namespace GUI {

class Textfield : public Widget {
    Util::UString m_content;
    int m_font_size = 15;
    Align m_alignment = Align::CenterLeft;

public:
    explicit Textfield(Container& c)
        : Widget(c) { }

    virtual void draw(GUI::Window& window) const override;

    Util::UString get_content() const { return m_content; }
    Textfield& set_content(Util::UString content) {
        m_content = std::move(content);
        return *this;
    }

    int get_font_size() const { return m_font_size; }
    void set_font_size(int font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; }
    void set_alignment(Align alignment) { m_alignment = alignment; }
};

}
