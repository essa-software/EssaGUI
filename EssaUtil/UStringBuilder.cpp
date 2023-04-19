#include "UStringBuilder.hpp"
#include <fmt/core.h>

namespace Util {

UStringBuilder::~UStringBuilder() {
    delete[] m_storage;
}

void UStringBuilder::reserve(size_t count) {
    if (count > m_capacity) {
        reallocate(count);
    }
}

void UStringBuilder::append(UString str) {
    append(str.span());
}

void UStringBuilder::append(uint32_t cp) {
    append({ &cp, 1 });
}

void UStringBuilder::append(std::span<uint32_t const> codepoints) {
    if (codepoints.empty()) {
        return;
    }
    auto needed_capacity = m_size + codepoints.size();
    if (needed_capacity > m_capacity) {
        reallocate(needed_capacity);
    }
    std::copy(codepoints.begin(), codepoints.end(), m_storage + m_size);
    m_size += codepoints.size();
}

void UStringBuilder::vappendff(fmt::string_view fmtstr, fmt::format_args args) {
    append(UString { fmt::vformat(fmtstr, args) });
}

UString UStringBuilder::build() const {
    return UString { codepoints() };
}

UString UStringBuilder::release_string() {
    auto string = UString::take_ownership(codepoints());
    m_storage = nullptr;
    m_capacity = 0;
    m_size = 0;
    return string;
}

void UStringBuilder::reallocate(size_t new_capacity) {
    auto old_storage = m_storage;
    if (new_capacity > 0) {
        auto old_capacity = m_capacity;

        m_storage = new uint32_t[new_capacity];
        if (old_storage) {
            std::copy(old_storage, old_storage + std::min(new_capacity, old_capacity), m_storage);
        }
    }
    else {
        m_storage = nullptr;
    }
    delete[] old_storage;
    m_capacity = new_capacity;
}

}
