#include "Parser.hpp"
#include "EssaGUI/eml/AST.hpp"
#include "EssaGUI/gfx/ResourceManager.hpp"
#include <EssaUtil/Config.hpp>
#include <EssaUtil/GenericParser.hpp>
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
    object.class_name = TRY(expect(TokenType::Identifier)).value();

    if (next_token_is(TokenType::Identifier)) {
        object.id = get()->value();
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
        std::visit([&object](auto decl) {
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
            decl);
        ignore_newlines_and_comments();
    }

    get(); // '}'

    return object;
}

Util::ParseErrorOr<ClassDefinition> Parser::parse_class_definition() {
    ignore_newlines_and_comments();

    TRY(expect(TokenType::KeywordDefine));

    ClassDefinition definition;
    definition.class_name = TRY(expect(TokenType::Identifier)).value();
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

    return Property {
        .name = name.value(),
        .value = value
    };
}

Util::ParseErrorOr<Value> Parser::parse_value() {
    auto token = peek();
    if (!token) {
        return error("Unexpected EOF in value");
    }
    switch (token->type()) {
    case TokenType::Number: {
        get();
        auto number = std::stoi(token->value());
        auto maybe_unit = peek();
        if (!maybe_unit)
            return error("Expected unit, got EOF");
        if (maybe_unit->type() == TokenType::PercentSign) {
            get();
            return Length { static_cast<float>(number), Length::Percent };
        }
        if (maybe_unit->type() == TokenType::Identifier && maybe_unit->value() == "px") {
            get();
            return Length { static_cast<float>(number), Length::Px };
        }
        if (maybe_unit->type() == TokenType::DoubleDot) {
            get();
            auto range_max = TRY(expect(TokenType::Number));
            return Range { static_cast<double>(number), std::stof(range_max.value()) };
        }
        return static_cast<double>(number);
    }
    case TokenType::Identifier: {
        get();
        if (token->value() == "auto")
            return Length { Length::Auto };
        if (token->value() == "initial")
            return Length { Length::Initial };
        if (token->value() == "true")
            return true;
        if (token->value() == "false")
            return false;
        return error("Invalid identifier in value, expected 'auto', 'initial', 'true' or 'false'");
    }
    case TokenType::String: {
        get();
        return Util::UString { token->value() };
    }
    case TokenType::At: {
        return TRY(parse_object());
    }
    case TokenType::KeywordAsset:
    case TokenType::KeywordExternal: {
        return TRY(parse_resource_id());
    }
    default:
        return error_in_already_read("Invalid value");
    }
}

Util::ParseErrorOr<Gfx::ResourceId> Parser::parse_resource_id() {
    auto keyword = get();
    TRY(expect(TokenType::ParenOpen));
    auto path = TRY(expect(TokenType::String));
    TRY(expect(TokenType::ParenClose));
    if (keyword->type() == TokenType::KeywordAsset) {
        return Gfx::ResourceId::asset(path.value());
    }
    if (keyword->type() == TokenType::KeywordExternal) {
        return Gfx::ResourceId::external(path.value());
    }
    return error_in_already_read("Expected 'asset' or 'external' in resource id");
}

void Parser::ignore_newlines_and_comments() {
    while (peek() && (peek()->type() == TokenType::NewLine || peek()->type() == TokenType::Comment))
        get();
}

}
