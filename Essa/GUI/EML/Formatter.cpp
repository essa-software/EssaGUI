#include "Formatter.hpp"
#include "Essa/GUI/EML/Lexer.hpp"
#include "EssaUtil/SourceLocation.hpp"
#include "EssaUtil/Stream/MemoryStream.hpp"
#include "EssaUtil/UStringBuilder.hpp"

namespace EML {

namespace {

bool should_have_space_between(Token const& lhs, Token const& rhs) {
    if (lhs.type() == TokenType::NewLine || rhs.type() == TokenType::NewLine) {
        return false;
    }
    if (lhs.type() == TokenType::Garbage || rhs.type() == TokenType::Garbage) {
        return false;
    }
    if (lhs.type() == TokenType::Comment || rhs.type() == TokenType::Comment) {
        return false;
    }
    // @Widget
    if (lhs.type() == TokenType::At && rhs.type() == TokenType::Identifier) {
        return false;
    }
    // property:
    if (lhs.type() == TokenType::Identifier && rhs.type() == TokenType::Colon) {
        return false;
    }
    // function(
    if (rhs.type() == TokenType::ParenOpen) {
        if (lhs.type() == TokenType::Identifier || lhs.type() == TokenType::KeywordAsset || lhs.type() == TokenType::KeywordExternal) {
            return false;
        }
    }
    // (xxx
    if (lhs.type() == TokenType::ParenOpen) {
        return false;
    }
    // xxx)
    if (rhs.type() == TokenType::ParenClose) {
        return false;
    }
    // 100px, 100%
    if (lhs.type() == TokenType::Number && (rhs.type() == TokenType::Identifier || rhs.type() == TokenType::PercentSign)) {
        return false;
    }
    return true;
}

bool force_newline_after(Token const& token) {
    if (token.type() == TokenType::CurlyClose) {
        return true;
    }
    if (token.type() == TokenType::Comment) {
        return true;
    }
    return false;
}

Util::UString serialize_token(Token const& token) {
    if (token.type() == TokenType::NewLine) {
        return "\n";
    }
    if (token.type() == TokenType::String) {
        // TODO: Properly escape
        return "\"" + token.value() + "\"";
    }
    if (token.type() == TokenType::Comment) {
        return "// " + token.value().trim_whitespace();
    }
    return token.value();
}

bool should_increase_indent(Token const& token) {
    if (token.type() == TokenType::CurlyOpen) {
        return true;
    }
    if (token.type() == TokenType::BraceOpen) {
        return true;
    }
    return false;
}

bool should_decrease_indent(Token const& token) {
    if (token.type() == TokenType::CurlyClose) {
        return true;
    }
    if (token.type() == TokenType::BraceClose) {
        return true;
    }
    return false;
}

}

std::string Formatter::format() const {
    Util::ReadableMemoryStream stream { { (uint8_t const*)m_input.data(), m_input.size() } };
    Lexer lexer { stream };
    // Lexing from memory should never fail.
    auto tokens = MUST(lexer.lex());

    // add dummy newline token
    tokens.push_back(Token { TokenType::NewLine, "", Util::SourceRange {} });

    if (tokens.size() < 2) {
        return m_input;
    }

    // print tokens
    // fmt::println("TOKENS:");
    // for (auto const& token : tokens) {
    //     fmt::println("{} {}", (int)token.type(), token.value().encode());
    // }

    Util::UStringBuilder builder;
    bool added_newline = false;
    size_t indent = 0;
    for (size_t s = 0; s < tokens.size() - 1; ++s) {
        auto const& token = tokens[s];
        auto const& next_token = tokens[s + 1];

        // fmt::println(
        //     "token={} next_token={} indent={} added_newline={}", token.value().encode(), next_token.value().encode(), indent,
        //     added_newline
        // );

        if (indent > 0 && should_decrease_indent(next_token)) {
            indent -= 1;
        }

        if (added_newline && token.type() != TokenType::NewLine) {
            // indent
            builder.appendff("{:>{}}", "", indent * 4);
        }
        added_newline = token.type() == TokenType::NewLine;
        builder.append(serialize_token(token));

        if (token.type() == TokenType::NewLine) {
            continue;
        }

        if (should_increase_indent(token)) {
            indent += 1;
        }

        if (force_newline_after(token) && next_token.type() != TokenType::NewLine) {
            builder.append("\n");
            added_newline = true;
        }
        else {
            if (should_have_space_between(token, next_token)) {
                builder.append(" ");
            }
        }
    }

    // Add last token (dummy newline) (only if it wasn't already added)
    if (tokens[tokens.size() - 2].type() != TokenType::NewLine) {
        builder.append(serialize_token(tokens.back()));
    }
    return builder.release_string().encode();
}

}
