#include "SyntaxHighlighter.hpp"

#include <EssaGUI/EML/Lexer.hpp>
#include <EssaGUI/TextEditing/SyntaxHighlighter.hpp>
#include <EssaUtil/Stream/MemoryStream.hpp>

namespace EML {

enum class HighlightStyle {
    Keyword,
    String,
    Number,
    Constant,
    Comment
};

std::vector<GUI::TextStyle> SyntaxHighlighter::styles() const {
    return {
        GUI::TextStyle { .color = Util::Color { 0x58d36fff } }, // Keyword
        GUI::TextStyle { .color = Util::Color { 0xbc894fff } }, // String
        GUI::TextStyle { .color = Util::Color { 0xdca6eaff } }, // Number
        GUI::TextStyle { .color = Util::Color { 0x26b1f2ff } }, // Constant
        GUI::TextStyle { .color = Util::Color { 0x679335ff } }, // Comment
    };
}

std::vector<GUI::StyledTextSpan> SyntaxHighlighter::spans(Util::UString const& input) const {
    auto encoded_input = input.encode();

    // TODO: Find a way to stream a UString. Maybe utf8 encode on the fly?
    Util::ReadableMemoryStream stream { { reinterpret_cast<uint8_t const*>(encoded_input.c_str()), encoded_input.size() } };
    EML::Lexer lexer { stream };
    auto tokens = MUST(lexer.lex());

    std::vector<GUI::StyledTextSpan> spans;

    auto create_span = [](EML::Token const& token, HighlightStyle style) {
        return GUI::StyledTextSpan { .span_start = token.start().offset, .span_size = token.end().offset - token.start().offset, .style_index = static_cast<size_t>(style) };
    };

    for (auto const& token : tokens) {
        if (token.type() == EML::TokenType::KeywordAsset || token.type() == EML::TokenType::KeywordDefine || token.type() == EML::TokenType::KeywordExternal) {
            spans.push_back(create_span(token, HighlightStyle::Keyword));
        }
        else if (token.type() == EML::TokenType::String) {
            spans.push_back(create_span(token, HighlightStyle::String));
        }
        else if (token.type() == EML::TokenType::Number || token.type() == EML::TokenType::PercentSign) {
            spans.push_back(create_span(token, HighlightStyle::Number));
        }
        else if (token.type() == EML::TokenType::Comment) {
            spans.push_back(create_span(token, HighlightStyle::Comment));
        }
        else if (token.type() == EML::TokenType::Identifier) {
            if (token.value() == "true" || token.value() == "false" || token.value() == "auto") {
                spans.push_back(create_span(token, HighlightStyle::Constant));
            }
            else if (token.value() == "px") {
                spans.push_back(create_span(token, HighlightStyle::Number));
            }
        }
    }

    return spans;
}

}
