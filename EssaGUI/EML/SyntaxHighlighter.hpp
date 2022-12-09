#pragma once

#include <EssaGUI/GUI/TextEditing/SyntaxHighlighter.hpp>

namespace EML {

class SyntaxHighlighter : public GUI::SyntaxHighlighter {
    virtual std::vector<GUI::TextStyle> styles() const override;
    virtual std::vector<GUI::StyledTextSpan> spans(Util::UString const& input) const override;
};

}
