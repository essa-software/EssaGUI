#include "Parser.hpp"

#include <Essa/GUI/EML/AST.hpp>
#include <Essa/GUI/EML/Lexer.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/GenericParser.hpp>
#include <EssaUtil/UString.hpp>
#include <variant>

namespace EML {

Util::ParseErrorOr<Declaration> Parser::parse_declaration() {
    auto token = peek();
    if (!token) {
        return error("Unexpected EOF");
    }
    if (token->type() == TokenType::At) {
        return TRY(parse_object());
    }
    if (token->type() == TokenType::KeywordDefine) {
        return TRY(parse_class_definition());
    }
    if (token->type() == TokenType::Identifier) {
        return TRY(parse_property());
    }
    return expected("'@', 'define' or identifier to start declaration", *token);
}

Util::ParseErrorOr<Object> Parser::parse_object() {
    ignore_newlines_and_comments();

    TRY(expect(TokenType::At));

    Object object;
    object.class_name = TRY(expect(TokenType::Identifier)).value().encode();

    if (next_token_is(TokenType::Identifier)) {
        object.id = get()->value().encode();
    }

    ignore_newlines_and_comments();

    if (!next_token_is(TokenType::CurlyOpen)) {
        return object;
    }

    get(); // '{'
    ignore_newlines_and_comments();

    // Declarations
    while (true) {
        if (next_token_is(TokenType::CurlyClose))
            break;
        auto decl = TRY(parse_declaration());
        std::visit(
            [&object](auto decl) {
                using Type = decltype(decl);
                if constexpr (std::is_same_v<Type, Object>) {
                    object.objects.push_back(std::move(decl));
                }
                else if constexpr (std::is_same_v<Type, ClassDefinition>) {
                    object.class_definitions.insert({ decl.class_name, std::move(decl) });
                }
                else if constexpr (std::is_same_v<Type, Property>) {
                    object.properties.insert({ decl.name, std::move(decl) });
                }
                else {
                    ESSA_UNREACHABLE;
                }
            },
            decl
        );
        ignore_newlines_and_comments();
    }

    get(); // '}'

    return object;
}

Util::ParseErrorOr<ClassDefinition> Parser::parse_class_definition() {
    ignore_newlines_and_comments();

    TRY(expect(TokenType::KeywordDefine));

    ClassDefinition definition;
    definition.class_name = TRY(expect(TokenType::Identifier)).value().encode();
    TRY(expect(TokenType::Colon));
    definition.base = TRY(parse_object());
    return definition;
}

Util::ParseErrorOr<Property> Parser::parse_property() {
    ignore_newlines_and_comments();

    // Name
    auto name = TRY(expect(TokenType::Identifier));

    TRY(expect(TokenType::Colon));

    // Value
    auto value = TRY(parse_value());

    return Property { .name = name.value().encode(), .value = value };
}

Util::ParseErrorOr<Value> Parser::parse_value() {
    auto token = peek();
    if (!token) {
        return error("Unexpected EOF in value");
    }
    switch (token->type()) {
    case TokenType::Hash: {
        get();
        return EML::Value(TRY(parse_hexcolor()));
    }
    case TokenType::Number: {
        get();
        auto number = MUST(token->value().parse<int>());
        auto maybe_unit = peek();
        if (!maybe_unit)
            return error("Expected unit, got EOF");
        if (maybe_unit->type() == TokenType::PercentSign) {
            get();
            return EML::Value(Util::Length { number, Util::Length::Percent });
        }
        if (maybe_unit->type() == TokenType::Identifier && maybe_unit->value() == "px") {
            get();
            return EML::Value(Util::Length { number, Util::Length::Px });
        }
        if (maybe_unit->type() == TokenType::DoubleDot) {
            get();
            auto range_max = TRY(expect(TokenType::Number));
            return EML::Value(Range { static_cast<double>(number), MUST(range_max.value().parse<float>()) });
        }
        return EML::Value(static_cast<double>(number));
    }
    case TokenType::Identifier: {
        get();
        if (token->value() == "auto")
            return EML::Value(Util::Length { Util::Length::Auto });
        if (token->value() == "initial")
            return EML::Value(Util::Length { Util::Length::Initial });
        if (token->value() == "true")
            return EML::Value(true);
        if (token->value() == "false")
            return EML::Value(false);
        return error("Invalid identifier in value, expected 'auto', 'initial', 'true' or 'false'");
    }
    case TokenType::String: {
        get();
        return EML::Value(Util::UString { token->value() });
    }
    case TokenType::At: {
        return EML::Value(TRY(parse_object()));
    }
    case TokenType::BraceOpen: {
        return EML::Value(TRY(parse_array()));
    }
    case TokenType::KeywordAsset:
    case TokenType::KeywordExternal: {
        return EML::Value(TRY(parse_resource_id()));
    }
    default:
        return error_in_already_read("Value cannot start with '" + token->value().encode() + "'");
    }
}

Util::ParseErrorOr<Array> Parser::parse_array() {
    TRY(expect(TokenType::BraceOpen));

    std::vector<Value> values;
    while (true) {
        auto value = TRY(parse_value());
        values.push_back(value);
        auto maybe_comma = peek();
        if (!maybe_comma) {
            return error("Unexpected EOF in array");
        }
        if (maybe_comma->type() != TokenType::Comma)
            break;
        get(); // ,
    }

    TRY(expect(TokenType::BraceClose));
    return Array { std::move(values) };
}

Util::ParseErrorOr<Gfx::ResourceId> Parser::parse_resource_id() {
    auto keyword = get();
    TRY(expect(TokenType::ParenOpen));
    auto path = TRY(expect(TokenType::String));
    TRY(expect(TokenType::ParenClose));
    if (keyword->type() == TokenType::KeywordAsset) {
        return Gfx::ResourceId::asset(path.value().encode());
    }
    if (keyword->type() == TokenType::KeywordExternal) {
        return Gfx::ResourceId::external(path.value().encode());
    }
    return error_in_already_read("Expected 'asset' or 'external' in resource id");
}

Util::ParseErrorOr<Util::Color> Parser::parse_hexcolor() {
    auto color_token = peek();
    std::string str;

    while (color_token->type() == TokenType::Identifier || color_token->type() == TokenType::Number) {
        str += color_token->value().encode();
        get();
        color_token = peek();
    }

    if (str.size() != 6 && str.size() != 8) {
        return error_in_already_read("Not valid hexadecimal number");
    }

    unsigned color = 0;

    for (const auto& c : str) {
        if (c >= '0' && c <= '9')
            color += c - 48;
        else if (c >= 'A' && c <= 'F') {
            color += c - 55;
        }
        else {
            return error_in_already_read("Not valid hexadecimal number");
        }
        color <<= 4;
    }

    if (str.size() == 6) {
        color <<= 8;
    }

    return Util::Color(color);
}

void Parser::ignore_newlines_and_comments() {
    while (peek() && (peek()->type() == TokenType::NewLine || peek()->type() == TokenType::Comment))
        get();
}

}
