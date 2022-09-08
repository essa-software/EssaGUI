#pragma once

#include <EssaUtil/GenericParser.hpp>
#include <EssaUtil/Stream/Stream.hpp>

namespace EML {

enum class TokenType {
    AngleBracketString, // <abc>
    Comment,            // //abc
    Identifier,         // abc
    Number,             // 123
    String,             // "abc"

    At,          // @
    BraceOpen,   // [
    BraceClose,  // ]
    Colon,       // :
    Comma,       // ,
    CurlyOpen,   // {
    CurlyClose,  // }
    DoubleDot,   // ..
    Hash,        // #
    ParenOpen,   // (
    ParenClose,  // )
    PercentSign, // %

    KeywordAsset,    // `asset`
    KeywordExternal, // `external`
    KeywordDefine,   // `define`

    NewLine, // \n
    Garbage, // anything else
};

using Token = Util::Token<TokenType>;

class Lexer : public Util::GenericLexer<TokenType> {
public:
    Lexer(Util::ReadableStream& stream)
        : Util::GenericLexer<TokenType>(stream) { }

    Util::OsErrorOr<std::vector<Token>> lex();
};

}
