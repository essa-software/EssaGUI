#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace Util {

class Any {
public:
    Any() = default;

    template<class U>
        requires(std::is_trivially_copyable_v<U>)
    Any(U const& u)
        : m_storage(sizeof(U)) {
        memcpy(m_storage.data(), &u, sizeof(U));
    }

    template<class U>
    U const& cast() const
        requires(std::is_trivially_copyable_v<U>)
    {
        assert(sizeof(U) <= m_storage.size());
        return *reinterpret_cast<U const*>(m_storage.data());
    }
    template<class U>
    U& cast()
        requires(std::is_trivially_copyable_v<U>)
    {
        assert(sizeof(U) <= m_storage.size());
        return *reinterpret_cast<U*>(m_storage.data());
    }

private:
    std::vector<uint8_t> m_storage;
};

template<class T>
    requires(std::is_trivially_copyable_v<T>)
class AnyWithBase {
public:
    AnyWithBase() = default;

    template<class U>
        requires(std::is_base_of_v<T, U>)
    AnyWithBase(U const& u)
        : m_storage(sizeof(U)) {
        memcpy(m_storage.data(), &u, sizeof(U));
    }

    template<class U>
        requires(std::is_base_of_v<T, U>)
    U const& cast() const {
        assert(sizeof(U) <= m_storage.size());
        return *reinterpret_cast<U const*>(m_storage.data());
    }
    template<class U>
        requires(std::is_base_of_v<T, U>)
    U& cast() {
        assert(sizeof(U) <= m_storage.size());
        return *reinterpret_cast<U*>(m_storage.data());
    }

    T& ref() { return cast<T>(); }
    T const& ref() const { return cast<T>(); }

private:
    std::vector<uint8_t> m_storage;
};

}
