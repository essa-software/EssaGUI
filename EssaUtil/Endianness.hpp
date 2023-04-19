#pragma once

#include <cstdint>

namespace Util {

template<class T>
constexpr T swap_endianness(T);

template<>
constexpr uint8_t swap_endianness<uint8_t>(uint8_t value) { return value; }
template<>
constexpr uint16_t swap_endianness<uint16_t>(uint16_t value) { return __builtin_bswap16(value); }
template<>
constexpr uint32_t swap_endianness<uint32_t>(uint32_t value) { return __builtin_bswap32(value); }
template<>
constexpr uint64_t swap_endianness<uint64_t>(uint64_t value) { return __builtin_bswap64(value); }

template<class T>
constexpr T convert_from_host_to_big_endian(T value) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#else
    return swap_endianness(value);
#endif
}

template<class T>
constexpr T convert_from_host_to_little_endian(T value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return value;
#else
    return swap_endianness(value);
#endif
}

template<class T>
constexpr T convert_from_big_to_host_endian(T value) {
    return convert_from_host_to_big_endian(value);
}

template<class T>
constexpr T convert_from_little_to_host_endian(T value) {
    return convert_from_host_to_little_endian(value);
}

}
