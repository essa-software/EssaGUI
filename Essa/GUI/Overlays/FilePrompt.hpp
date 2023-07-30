#pragma once

#include "FileExplorer.hpp"
#include "ToolWindow.hpp"

#include <optional>

namespace GUI {

class FilePrompt : public ToolWindow {
public:
    explicit FilePrompt(MDI::Host& window, Util::UString help_text, Util::UString window_title, Util::UString placeholder);

    std::optional<Util::UString> result() const { return m_result; }

    void add_desired_extension(std::string ext) { m_extensions.push_back(ext); }

private:
    std::optional<Util::UString> m_result;

    std::vector<std::string> m_extensions;
};

FilePrompt* file_prompt(MDI::Host&, Util::UString help_text, Util::UString window_title = "Prompt", Util::UString placeholder = "");

}
