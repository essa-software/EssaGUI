#pragma once

#include "FileExplorer.hpp"
#include "ToolWindow.hpp"

#include <optional>

namespace GUI {

class FilePrompt : public ToolWindow {
public:
    explicit FilePrompt(sf::RenderWindow& wnd, sf::String help_text, sf::String window_title, sf::String placeholder);

    std::optional<sf::String> result() const { return m_result; }

    void add_desired_extension(std::string ext){m_extensions.push_back(ext);}

private:
    std::optional<sf::String> m_result;

    std::vector<std::string> m_extensions;
};

FilePrompt* file_prompt(sf::String help_text, sf::String window_title = "Prompt", sf::String placeholder = "");

}
