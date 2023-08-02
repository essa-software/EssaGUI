#pragma once

#include <Essa/GUI/WindowRoot.hpp>

namespace GUI {

class Button;

class MessageBox : public WindowRoot {
public:
    enum class Buttons { YesNo, Ok };

    explicit MessageBox(WidgetTreeRoot& wnd, Util::UString message, Util::UString title, Buttons buttons);

    enum class ButtonRole { None, Yes, No, Ok };

    ButtonRole exec();

private:
    virtual Widget::EventHandlerResult handle_event(GUI::Event const&) override;

    ButtonRole m_clicked_button = ButtonRole::None;
    Button* m_default_button = nullptr;
};

MessageBox::ButtonRole message_box(HostWindow&, Util::UString message, Util::UString title, MessageBox::Buttons buttons);

}
