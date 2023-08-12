#pragma once

#include "ToolWindow.hpp"

#include <optional>

namespace GUI {

class Prompt : public WindowRoot {
public:
    explicit Prompt(WidgetTreeRoot& window, Util::UString help_text, Util::UString window_title, Util::UString placeholder);

    std::optional<Util::UString> result() const { return m_result; }

private:
    virtual Widget::EventHandlerResult handle_event(llgl::Event const&) override;

    std::optional<Util::UString> m_result;
    GUI::TextButton* m_ok_button = nullptr;
    GUI::TextButton* m_cancel_button = nullptr;
};

std::optional<Util::UString>
prompt(HostWindow&, Util::UString help_text, Util::UString window_title = "Prompt", Util::UString placeholder = "");

}
