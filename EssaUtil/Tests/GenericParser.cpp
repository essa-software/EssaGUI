#include <EssaUtil/Testing.hpp>

#include <EssaUtil/GenericParser.hpp>
#include <EssaUtil/Stream/MemoryStream.hpp>
#include <EssaUtil/Stream/Stream.hpp>
#include <list>

enum class TestTokenType { Text, Number, ParenOpen, ParenClose, Comma, Garbage };

using TestToken = Token<TestTokenType>;

class TestLexer : public GenericLexer<TestTokenType> {
public:
    explicit TestLexer(ReadableStream& stream)
        : GenericLexer<TestTokenType>(stream) { }

    OsErrorOr<std::vector<TestToken>> lex() {
        std::vector<TestToken> tokens;
        TRY(ignore_whitespace());
        while (!is_eof()) {
            auto next = TRY(peek()).value();
            auto start = location();
            if (isalpha(next) || next == U'ą') {
                auto string = TRY(consume_while([](uint32_t cp) { return isalpha(cp) || cp == U'ą'; }));
                tokens.push_back(create_token(TestTokenType::Text, string, start));
            }
            else if (isdigit(next)) {
                auto string = TRY(consume_while(isdigit));
                tokens.push_back(create_token(TestTokenType::Number, string, start));
            }
            else if (next == ',') {
                TRY(consume());
                tokens.push_back(create_token(TestTokenType::Comma, ",", start));
            }
            else if (next == '(') {
                TRY(consume());
                tokens.push_back(create_token(TestTokenType::ParenOpen, "(", start));
            }
            else if (next == ')') {
                TRY(consume());
                tokens.push_back(create_token(TestTokenType::ParenClose, ")", start));
            }
            else {
                TRY(consume());
                tokens.push_back(create_token(TestTokenType::Garbage, Util::UString { next }, start));
            }
            TRY(ignore_whitespace());
        }
        return tokens;
    }
};

namespace AST {

struct Array;

using Expression = std::variant<Array, int, UString>;

struct Array {
    std::list<Expression> subexpressions;
};

template<class OutputIt> void print(OutputIt out, int i) { fmt::format_to(out, "{}", i); }

template<class OutputIt> void print(OutputIt out, Util::UString const& str) { fmt::format_to(out, "'{}'", str.encode()); }

template<class OutputIt> void print(OutputIt out, Expression const& expr);

template<class OutputIt> void print(OutputIt out, Array const& array) {
    size_t i = 0;
    fmt::format_to(out, "(");
    for (auto const& expr : array.subexpressions) {
        print(out, expr);
        if (i != array.subexpressions.size() - 1)
            fmt::format_to(out, ", ");
        i++;
    }
    fmt::format_to(out, ")");
}

template<class OutputIt> void print(OutputIt out, Expression const& expr) {
    std::visit([out](auto const& value) { print(out, value); }, expr);
}

}

class TestParser : public GenericParser<TestTokenType> {
public:
    explicit TestParser(std::vector<TestToken> tokens)
        : GenericParser<TestTokenType>(std::move(tokens)) { }

    ParseErrorOr<AST::Expression> parse_expression();
    ParseErrorOr<AST::Array> parse_array();
    ParseErrorOr<int> parse_number();
    ParseErrorOr<UString> parse_text();
};

ParseErrorOr<AST::Expression> TestParser::parse_expression() {
    auto token = peek();
    if (!token)
        return error("Unexpected EOF in expression");
    if (token->type() == TestTokenType::ParenOpen) {
        return TRY(parse_array());
    }
    if (token->type() == TestTokenType::Number) {
        return TRY(parse_number());
    }
    if (token->type() == TestTokenType::Text) {
        return TRY(parse_text());
    }
    return expected("array, number or text", *token);
}

ParseErrorOr<AST::Array> TestParser::parse_array() {
    TRY(expect(TestTokenType::ParenOpen));

    AST::Array result;

    while (true) {
        auto expr = TRY(parse_expression());
        result.subexpressions.push_back(std::move(expr));

        if (!peek())
            return error("Unexpected EOF in array");

        if (peek()->type() != TestTokenType::Comma)
            break;
        else
            get();
    }

    TRY(expect(TestTokenType::ParenClose));
    return result;
}

ParseErrorOr<int> TestParser::parse_number() {
    auto value = TRY(expect(TestTokenType::Number)).value();
    return value.parse<int>().map_error([](OsError&& err) { return ParseError { .message = std::string { err.function }, .location = {} }; }
    );
}

ParseErrorOr<UString> TestParser::parse_text() { return TRY(expect(TestTokenType::Text)).value(); }

ErrorOr<void, __TestSuite::TestError> expect_success(std::string_view input, std::string_view result) {
    ReadableMemoryStream stream = ReadableMemoryStream::from_string(input);

    TestLexer lexer { stream };
    auto tokens = lexer.lex();
    EXPECT_NO_ERROR(tokens);

    TestParser parser(tokens.release_value());
    auto parsed_expr = parser.parse_expression();

    if (parsed_expr.is_error()) {
        auto error = parsed_expr.release_error();
        fmt::print(stderr, "{}\n", error.message);
        return __TestSuite::TestError { "success for " + std::string { input }, "GenericParser.cpp", __LINE__ };
    }
    else {
        std::string out;
        print(std::back_inserter(out), parsed_expr.release_value());
        EXPECT_EQ(out, result);
    }
    return {};
}

ErrorOr<void, __TestSuite::TestError> expect_error(std::string_view input, std::string_view error_message) {
    ReadableMemoryStream stream = ReadableMemoryStream::from_string(input);

    TestLexer lexer { stream };
    auto tokens = lexer.lex();
    EXPECT_NO_ERROR(tokens);

    TestParser parser(tokens.release_value());
    auto parsed_expr = parser.parse_expression();

    if (parsed_expr.is_error()) {
        auto error = parsed_expr.release_error();
        EXPECT_EQ(error.message, error_message);
        return {};
    }
    else {
        return __TestSuite::TestError { "error for " + std::string { input }, "GenericParser.cpp", __LINE__ };
    }
    return {};
}

TEST_CASE(generic_parser) {
    TRY(expect_success(
        "(1,2,4,  a  , b,  (other, array ), (more, (nested, arrays, (yay))  ) )",
        "(1, 2, 4, 'a', 'b', ('other', 'array'), ('more', ('nested', 'arrays', ('yay'))))"
    ));
    TRY(expect_success("(ąąąą)", "('ąąąą')"));
    TRY(expect_error("#", "Expected array, number or text, got '#'"));
    TRY(expect_error("(", "Unexpected EOF in expression"));
    return {};
}
