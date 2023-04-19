#pragma once

#include "NonCopyable.hpp"
#include "Vector.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>

namespace Util {

template<class T, size_t X, size_t Y>
class [[deprecated("use MultidimensionalArray instead")]] DynamicArray2D : public NonCopyable {
public:
    using Type = T;
    static constexpr size_t SizeX = X;
    static constexpr size_t SizeY = Y;

    DynamicArray2D(Type const& v = {}) {
        fill(v);
    }

    DynamicArray2D(DynamicArray2D const&) = delete;
    DynamicArray2D& operator=(DynamicArray2D const&) = delete;

    DynamicArray2D(DynamicArray2D && arr) {
        *this = std::move(arr);
    }
    DynamicArray2D& operator=(DynamicArray2D&& arr) {
        if (this == &arr)
            return *this;
        if (m_storage)
            delete[] m_storage;
        m_storage = std::exchange(arr.m_storage, nullptr);
        return *this;
    }

    ~DynamicArray2D() { delete[] m_storage; }

    Util::Vector2<size_t> dimensions() const { return { SizeX, SizeY }; }
    size_t size() const { return SizeX * SizeY; }

    void fill(Type const& fill) {
        if (!m_storage)
            m_storage = new Type[size()];
        std::fill(m_storage, m_storage + size(), fill);
    }

    Type& ref(size_t x, size_t y) {
        assert(m_storage);
        assert(x < X && y < Y);
        return *cell(x, y);
    }

    Type const& ref(size_t x, size_t y) const {
        assert(m_storage);
        assert(x < X && y < Y);
        return *cell(x, y);
    }

    Type get(size_t x, size_t y) const {
        if (!m_storage) {
            // Array doesn't take memory but appears default-initialized here.
            return T {};
        }
        assert(x < X && y < Y);
        return *cell(x, y);
    }

    void set(size_t x, size_t y, T value) {
        if (!m_storage)
            fill({});
        assert(x < X && y < Y);
        *cell(x, y) = std::move(value);
    }

private:
    Type* cell(size_t x, size_t y) { return &m_storage[coords_to_index(x, y)]; }
    Type const* cell(size_t x, size_t y) const { return &m_storage[coords_to_index(x, y)]; }
    size_t coords_to_index(size_t x, size_t y) const { return y * SizeX + x; }

    Type* m_storage = nullptr;
};

}
