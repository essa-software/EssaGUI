#pragma once

#include <Essa/GUI/WindowRoot.hpp>

namespace GUI {

class Button;

class MessageBox : public WindowRoot {
public:
    enum class Buttons { YesNo, Ok };
    enum class Icon {
        None,
        Info,
        Warning,
        Error,
    };

    struct Settings {
        Buttons buttons = Buttons::Ok;
        Icon icon = Icon::None;
    };

    explicit MessageBox(WidgetTreeRoot& wnd, Util::UString message, Util::UString title, Settings);

    enum class ButtonRole { None, Yes, No, Ok };

    ButtonRole clicked_button() const {
        return m_clicked_button;
    }

private:
    virtual Widget::EventHandlerResult handle_event(llgl::Event const&) override;

    ButtonRole m_clicked_button = ButtonRole::None;
    Button* m_default_button = nullptr;
};

MessageBox::ButtonRole message_box(HostWindow* host_window, Util::UString message, Util::UString title, MessageBox::Settings settings);
[[deprecated]] MessageBox::ButtonRole message_box(HostWindow&, Util::UString message, Util::UString title, MessageBox::Buttons buttons);

}
