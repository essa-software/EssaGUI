#pragma once

#include "SourceLocation.hpp"
#include "Stream/Reader.hpp"
#include "Stream/Stream.hpp"

#include <string>
#include <type_traits>
#include <vector>

namespace Util {

template<class T>
requires(std::is_enum_v<T>) class Token {
public:
    using Type = T;

    Token(Type type, UString value, SourceRange range)
        : m_type(type)
        , m_value(std::move(value))
        , m_range(range) { }

    Type type() const { return m_type; }
    UString value() const { return m_value; }
    SourceRange range() const { return m_range; }
    SourceLocation start() const { return m_range.start; }
    SourceLocation end() const { return m_range.end; }

private:
    Type m_type;
    UString m_value;
    SourceRange m_range;
};

template<class T>
class GenericLexer : protected TextReader {
public:
    explicit GenericLexer(ReadableStream& stream)
        : TextReader(stream) { }

protected:
    auto create_token(T type, UString value, SourceLocation start) requires(!std::is_same_v<T, void>) {
        return Token<T> { type, std::move(value), { start, location() } };
    }
};

struct ParseError {
    std::string message;
    SourceRange location;
};

template<class T>
using ParseErrorOr = ErrorOr<T, ParseError>;

template<class T>
class GenericParser {
public:
    GenericParser(std::vector<Token<T>> tokens)
        : m_tokens(std::move(tokens)) { }

protected:
    bool is_eof() const {
        return m_offset >= m_tokens.size();
    }

    size_t offset() const { return m_offset; }

    SourceRange range(size_t start, size_t size) {
        assert(start + size < m_tokens.size());
        return SourceRange { .start = m_tokens[start].start(), .end = m_tokens[start + size - 1].end() };
    }

    Token<T> const* get() {
        if (is_eof())
            return nullptr;
        return &m_tokens[m_offset++];
    }
    Token<T> const* peek() const {
        if (is_eof())
            return nullptr;
        return &m_tokens[m_offset];
    }

    bool next_token_is(T type) const {
        return peek() && peek()->type() == type;
    }

    ParseErrorOr<Token<T>> expect(T type) {
        auto token = get();
        if (!token) {
            return error("Unexpected EOF");
        }
        if (token->type() != type) {
            return expected_in_already_read(fmt::format("token of type {}", (int)type), *token);
        }
        return *token;
    }

    ParseError error(std::string message) {
        if (m_tokens.empty())
            return ParseError { .message = message, .location = {} };
        return ParseError {
            .message = message,
            .location = is_eof()
                ? m_tokens[m_offset - 1].range()
                : m_tokens[m_offset].range()
        };
    }

    ParseError error(std::string message, size_t token) {
        assert(token < m_tokens.size());
        return ParseError {
            .message = message,
            .location = m_tokens[token].range()
        };
    }

    ParseError error_in_already_read(std::string message) {
        if (m_tokens.empty())
            return ParseError { .message = message, .location = {} };
        assert(m_offset > 0);
        return ParseError {
            .message = message,
            .location = m_tokens[m_offset - 1].range()
        };
    }

    ParseError expected(std::string what, Token<T> got) {
        return error("Expected " + what + ", got '" + got.value().encode() + "'");
    }

    ParseError expected_in_already_read(std::string what, Token<T> got) {
        return error_in_already_read("Expected " + what + ", got '" + got.value().encode() + "'");
    }

private:
    std::vector<Token<T>> m_tokens;
    size_t m_offset { 0 };
};

}
