#pragma once

#include "TextAlign.hpp"
#include "Widget.hpp"
#include <EssaUtil/Units.hpp>

namespace GUI {

class Textfield : public Widget {
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

    float padding() const { return m_padding; }
    void set_padding(float p) { m_padding = p; }

    Util::Rectf text_rect() const;

private:
    virtual LengthVector initial_size() const override;

    Util::UString m_content;
    int m_font_size = theme().label_font_size;
    Align m_alignment = Align::CenterLeft;
    float m_padding = 5;
};

}
