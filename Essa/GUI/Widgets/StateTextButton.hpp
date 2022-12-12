#pragma once

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/TextAlign.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <functional>
#include <vector>

namespace GUI {

template<typename T>
class StateTextButton : public Widget {
    struct State {
        Util::Color bg_color, fg_color, text_color;
        T state;
    };

public:
    void add_state(Util::UString content, T state, Util::Color bg_color, Util::Color fg_color = Util::Colors::Blue, Util::Color text_color = Util::Colors::White);

    CREATE_VALUE(size_t, font_size, 20)
    CREATE_VALUE(Align, alignment, Align::CenterLeft);

    std::pair<Util::UString, State> get_element_from_index(unsigned index) const;

    T state() const { return m_states[m_index].second.state; }

    std::function<void(T)> on_change;

    void set_index(unsigned index);

private:
    virtual void draw(Gfx::Painter& window) const override;
    virtual EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&) override;

    std::vector<std::pair<Util::UString, State>> m_states;

    size_t m_index = 0;
};

}

// These ifndefs are just so that clangd doesn't complain that much
#ifndef STATE_TEXT_BUTTON_INL
#    define STATE_TEXT_BUTTON_HPP
#    include "StateTextButton.inl"
#endif
