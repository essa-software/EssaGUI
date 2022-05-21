#pragma once

#include "ToolWindow.hpp"

#include <optional>

namespace GUI {

class Prompt : public ToolWindow {
public:
    explicit Prompt(sf::RenderWindow& wnd, sf::String help_text, sf::String window_title, sf::String placeholder);

    std::optional<sf::String> result() const { return m_result; }

private:
    std::optional<sf::String> m_result;
};

std::optional<sf::String> prompt(sf::String help_text, sf::String window_title = "Prompt", sf::String placeholder = "");

}
