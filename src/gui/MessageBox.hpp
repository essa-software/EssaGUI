#pragma once

#include "ToolWindow.hpp"

namespace GUI {

class Button;

class MessageBox : public ToolWindow {
public:
    enum class Buttons {
        YesNo,
        Ok
    };

    explicit MessageBox(sf::RenderWindow& wnd, sf::String message, sf::String title, Buttons buttons);

    enum class ButtonRole {
        None,
        Yes,
        No,
        Ok
    };

    ButtonRole exec() {
        run();
        return m_clicked_button;
    }

private:
    virtual void handle_event(sf::Event) override;

    ButtonRole m_clicked_button = ButtonRole::None;
    Button* m_default_button = nullptr;
};

MessageBox::ButtonRole message_box(sf::String message, sf::String title, MessageBox::Buttons buttons);

}
