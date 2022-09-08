#pragma once

#include "AST.hpp"
#include "Lexer.hpp"

#include <EssaUtil/GenericParser.hpp>

namespace EML {

class Parser : public Util::GenericParser<TokenType> {
public:
    explicit Parser(std::vector<Token> tokens)
        : Util::GenericParser<TokenType>(std::move(tokens)) { }

    Util::ParseErrorOr<Object> parse_object();

private:
    Util::ParseErrorOr<Declaration> parse_declaration();
    Util::ParseErrorOr<ClassDefinition> parse_class_definition();
    Util::ParseErrorOr<Property> parse_property();
    Util::ParseErrorOr<Gfx::ResourceId> parse_resource_id();
    Util::ParseErrorOr<Value> parse_value();
    Util::ParseErrorOr<Array> parse_array();

    void ignore_newlines_and_comments();
};

}
