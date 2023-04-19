#pragma once

#include <cctype>
#include <cstdint>

namespace Util {

enum class CharacterType {
    Unknown,
    Identifier,
    Punctuation,
    Whitespace
};

constexpr CharacterType character_type(uint32_t codepoint) {
    if (std::isalnum(codepoint) || codepoint == '_')
        return CharacterType::Identifier;
    if (std::ispunct(codepoint))
        return CharacterType::Punctuation;
    if (std::isspace(codepoint))
        return CharacterType::Whitespace;
    return CharacterType::Unknown;
}

}
