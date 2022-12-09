#pragma once

#include <EssaGUI/TextEditing/SyntaxHighlighter.hpp>

namespace EML {

class SyntaxHighlighter : public GUI::SyntaxHighlighter {
    virtual std::vector<GUI::TextStyle> styles() const override;
    virtual std::vector<GUI::StyledTextSpan> spans(Util::UString const& input) const override;
};

}
