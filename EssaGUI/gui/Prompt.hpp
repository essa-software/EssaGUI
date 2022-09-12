#pragma once

#include "ToolWindow.hpp"

#include <optional>

namespace GUI {

class Prompt : public ToolWindow {
public:
    explicit Prompt(Util::UString help_text, Util::UString window_title, Util::UString placeholder);

    std::optional<Util::UString> result() const { return m_result; }

private:
    std::optional<Util::UString> m_result;
};

std::optional<Util::UString> prompt(Util::UString help_text, Util::UString window_title = "Prompt", Util::UString placeholder = "");

}
