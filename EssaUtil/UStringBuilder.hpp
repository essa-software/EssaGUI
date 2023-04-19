#pragma once

#include "UString.hpp"
#include <cstddef>
#include <cstdint>

namespace Util {

class UStringBuilder {
public:
    UStringBuilder() = default;
    UStringBuilder(UStringBuilder const&) = delete;
    ~UStringBuilder();

    // Reallocate an internal buffer if it will become bigger. Size smaller
    // than current capacity is ignored. This doesn't initialize the data.
    void reserve(size_t);

    void append(UString);
    void append(uint32_t);
    void append(std::span<uint32_t const>);

    template<class... Args>
    void appendff(fmt::format_string<Args...> fmtstr, Args&&... args) {
        vappendff(fmtstr, fmt::make_format_args(args...));
    }

    void vappendff(fmt::string_view fmtstr, fmt::format_args args);

    // Return a copy of built string
    UString build() const;

    // Return a string and clear this builder. This allows to avoid unnecessary
    // copying.
    UString release_string();

    std::span<uint32_t const> codepoints() const { return { m_storage, m_size }; }

private:
    // Allocate a new array with specified capacity, copying previous data
    // that fits that new array. This doesn't touch size.
    void reallocate(size_t capacity);

    uint32_t* m_storage = nullptr;
    size_t m_capacity = 0;

    size_t m_size = 0;
};

}
