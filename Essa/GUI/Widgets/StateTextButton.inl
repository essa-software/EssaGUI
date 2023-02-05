// Included from StateTextButton.hpp
#ifndef STATE_TEXT_BUTTON_HPP
// These ifndefs are just so that clangd doesn't complain that much
#    define STATE_TEXT_BUTTON_INL
#    include "StateTextButton.hpp"
#endif

#include <Essa/GUI/Graphics/Text.hpp>

#include <Essa/GUI/Widgets/Widget.hpp>

namespace GUI {

template<typename T>
std::pair<Util::UString, typename StateTextButton<T>::State> StateTextButton<T>::get_element_from_index(unsigned index) const {
    unsigned i = 0;

    for (const auto& p : m_states) {
        if (i == index)
            return p;
        i++;
    }
    return {};
}

template<typename T>
void StateTextButton<T>::add_state(Util::UString content, T state, Util::Color bg_color, Util::Color fg_color, Util::Color text_color) {
    m_states.push_back({ content, { bg_color, fg_color, text_color, state } });
}

template<typename T>
void StateTextButton<T>::set_index(unsigned int index) {
    if (index >= m_states.size())
        m_index = m_states.size() - 1;
    else
        m_index = index;
    on_change(state());
}

template<typename T>
Widget::EventHandlerResult StateTextButton<T>::on_mouse_button_press(Event::MouseButtonPress const&) {
    m_index++;
    if (m_index >= m_states.size())
        m_index = 0;

    on_change(state());
    return Widget::EventHandlerResult::NotAccepted;
}

template<typename T>
void StateTextButton<T>::draw(Gfx::Painter& painter) const {
    Gfx::RectangleDrawOptions bg;
    bg.fill_color = get_element_from_index(m_index).second.bg_color;
    bg.outline_color = get_element_from_index(m_index).second.fg_color;
    bg.outline_thickness = 3;
    painter.deprecated_draw_rectangle(local_rect(), bg);

    Gfx::Text text { get_element_from_index(m_index).first, Application::the().font() };
    text.set_font_size(theme().label_font_size);
    text.set_fill_color(get_element_from_index(m_index).second.text_color);
    text.align(Align::Center, local_rect());
    text.draw(painter);
}

}
