#pragma once
#include "TextAlign.hpp"
#include "Widget.hpp"
#include "Application.hpp"
#include <vector>
#include <functional>

namespace GUI {

template <typename T>
class StateTextButton : public Widget {
    struct State{
        Util::Color bg_color, fg_color, text_color;
        T state;
    };
public:
    explicit StateTextButton(Container& c);

    void add_state(Util::UString content, T state, Util::Color bg_color, Util::Color fg_color = Util::Colors::blue, Util::Color text_color = Util::Colors::white);

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    std::pair<Util::UString, State> get_element_from_index(unsigned index) const;

    T state() const;

    std::function<void(T)> on_change;

    void set_bg_color(Util::Color color);
    void set_fg_color(Util::Color color);
    void set_text_color(Util::Color color);

    void set_index(unsigned index);

private:
    virtual void draw(GUI::Window& window) const override;
    virtual void handle_event(Event& event) override;

    std::vector<std::pair<Util::UString, State>> m_states;

    unsigned m_font_size = 20, m_index = 0;
    Align m_alignment = Align::CenterLeft;
};

}

// These ifndefs are just so that clangd doesn't complain that much
#ifndef STATE_TEXT_BUTTON_INL
#define STATE_TEXT_BUTTON_HPP
#include "StateTextButton.inl"
#endif
