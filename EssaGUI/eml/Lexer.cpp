#include "Lexer.hpp"

namespace EML {

Util::OsErrorOr<std::vector<Token>> Lexer::lex() {
    std::vector<Token> tokens;
    TRY(ignore_whitespace());
    while (!is_eof()) {
        auto next = TRY(peek()).value();
        auto start = location();
        if (isalpha(next) || next == '_') {
            auto string = TRY(consume_while([](char c) { return isalpha(c) || c == '_'; }));
            if (string == "asset")
                tokens.push_back(create_token(TokenType::KeywordAsset, string, start));
            else if (string == "define")
                tokens.push_back(create_token(TokenType::KeywordDefine, string, start));
            else if (string == "external")
                tokens.push_back(create_token(TokenType::KeywordExternal, string, start));
            else
                tokens.push_back(create_token(TokenType::Identifier, string, start));
        }
        else if (isdigit(next)) {
            auto string = TRY(consume_while(isdigit));
            tokens.push_back(create_token(TokenType::Number, string, start));
        }
        else if (next == '"') {
            TRY(consume());
            // TODO: Escaping
            auto string = TRY(consume_until('"'));
            tokens.push_back(create_token(TokenType::String, string, start));
        }
        else if (next == '<') {
            TRY(consume());
            auto string = TRY(consume_until('>'));
            tokens.push_back(create_token(TokenType::String, string, start));
        }
        else if (next == '/') {
            TRY(consume());
            if (TRY(peek()) == '/') {
                TRY(consume_until('\n'));
                tokens.push_back(create_token(TokenType::Comment, "comment", start));
            }
            else {
                tokens.push_back(create_token(TokenType::Garbage, "/", start));
            }
        }
        else {
            switch (next) {
            case '@':
                TRY(consume());
                tokens.push_back(create_token(TokenType::At, "@", start));
                break;
            case ':':
                TRY(consume());
                tokens.push_back(create_token(TokenType::Colon, ":", start));
                break;
            case ',':
                TRY(consume());
                tokens.push_back(create_token(TokenType::Comma, ",", start));
                break;
            case '{':
                TRY(consume());
                tokens.push_back(create_token(TokenType::CurlyOpen, "{", start));
                break;
            case '}':
                TRY(consume());
                tokens.push_back(create_token(TokenType::CurlyClose, "}", start));
                break;
            case '.':
                TRY(consume());
                if (TRY(peek()) == '.') {
                    TRY(consume());
                    tokens.push_back(create_token(TokenType::DoubleDot, "..", start));
                }
                else {
                    tokens.push_back(create_token(TokenType::Garbage, ".", start));
                }
                break;
            case '#':
                TRY(consume());
                tokens.push_back(create_token(TokenType::Hash, "#", start));
                break;
            case '(':
                TRY(consume());
                tokens.push_back(create_token(TokenType::ParenOpen, "(", start));
                break;
            case ')':
                TRY(consume());
                tokens.push_back(create_token(TokenType::ParenClose, ")", start));
                break;
            case '%':
                TRY(consume());
                tokens.push_back(create_token(TokenType::PercentSign, "%", start));
                break;
            case '\n':
                TRY(consume());
                tokens.push_back(create_token(TokenType::NewLine, "\\n", start));
                break;
            default:
                TRY(consume());
                fmt::print("GARBAGE!! '{:c}' @ {}\n", next, location().offset);
                tokens.push_back(create_token(TokenType::Garbage, std::string { (char)next }, start));
                break;
            }
        }
        TRY(consume_while([](char c) {
            return isspace(c) && c != '\n';
        }));
    }
    return tokens;
}

}
