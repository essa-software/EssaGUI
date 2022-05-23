// Included from StateTextButton.hpp
#ifndef STATE_TEXT_BUTTON_HPP
// These ifndefs are just so that clangd doesn't complain that much
#    define STATE_TEXT_BUTTON_INL
#    include "EssaGUI/gfx/SFMLWindow.hpp"
#    include "StateTextButton.hpp"
#endif

#include "Widget.hpp"
#include <SFML/Window/Event.hpp>

namespace GUI {

template<typename T>
StateTextButton<T>::StateTextButton(Container& c)
    : Widget(c) {
}

template<typename T>
std::pair<std::string, typename StateTextButton<T>::State> StateTextButton<T>::get_element_from_index(unsigned index) const {
    unsigned i = 0;

    for (const auto& p : m_states) {
        if (i == index)
            return p;
        i++;
    }
    return {};
}

template<typename T>
void StateTextButton<T>::add_state(sf::String content, T state, sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
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
void StateTextButton<T>::handle_event(Event& event) {
    if (event.type() == sf::Event::MouseButtonPressed) {
        if (is_hover()) {
            m_index++;
            if (m_index >= m_states.size())
                m_index = 0;

            on_change(state());
        }
    }
}

template<typename T>
void StateTextButton<T>::draw(GUI::SFMLWindow& window) const {
    GUI::RectangleDrawOptions bg;
    bg.fill_color = get_element_from_index(m_index).second.bg_color;
    bg.outline_color = get_element_from_index(m_index).second.fg_color;
    bg.outline_thickness = 3;
    window.draw_rectangle(local_rect(), bg);

    GUI::TextDrawOptions text_options {
        .font_size = 15,
        .text_align = m_alignment,
    };
    text_options.fill_color = get_element_from_index(m_index).second.text_color;
    window.draw_text_aligned_in_rect(get_element_from_index(m_index).first, local_rect(), Application::the().font, text_options);
}

template<typename T>
T StateTextButton<T>::state() const {
    return get_element_from_index(m_index).second.state;
}

template<typename T>
void StateTextButton<T>::set_bg_color(sf::Color color) {
    for (auto& state : m_states)
        state.second.bg_color = color;
}

template<typename T>
void StateTextButton<T>::set_fg_color(sf::Color color) {
    for (auto& state : m_states)
        state.second.fg_color = color;
}

template<typename T>
void StateTextButton<T>::set_text_color(sf::Color color) {
    for (auto& state : m_states)
        state.second.text_color = color;
}

}
