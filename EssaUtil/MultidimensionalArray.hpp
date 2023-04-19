#pragma once

#include "NonCopyable.hpp"
#include "Vector.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace Util {

template<class T, size_t... Dimensions>
class MultidimensionalArray {
public:
    static constexpr size_t Rank = sizeof...(Dimensions);
    using Type = T;
    using Vector = Detail::DeprecatedVector<Rank, size_t>;

    template<size_t Dim>
    static size_t dimension() {
        return std::get<Dim>(std::tuple { Dimensions... });
    }

    MultidimensionalArray(Type const& v = {}) {
        m_storage = new Type[size()];
        fill(v);
    }

    MultidimensionalArray(MultidimensionalArray const&) = delete;
    MultidimensionalArray& operator=(MultidimensionalArray const&) = delete;

    MultidimensionalArray(MultidimensionalArray&& arr) {
        *this = std::move(arr);
    }
    MultidimensionalArray& operator=(MultidimensionalArray&& arr) {
        if (this == &arr)
            return *this;
        if (m_storage)
            delete[] m_storage;
        m_storage = std::exchange(arr.m_storage, nullptr);
        return *this;
    }

    ~MultidimensionalArray() { delete[] m_storage; }

    Vector dimensions() const { return { Dimensions... }; }
    size_t size() const { return (Dimensions * ...); }

    void fill(Type const& fill) {
        std::fill(m_storage, m_storage + size(), fill);
    }

    template<std::convertible_to<size_t>... Dim>
    Type& ref(Dim... index) {
        assert(((index < Dimensions) && ...));
        return *cell(index...);
    }

    template<std::convertible_to<size_t>... Dim>
    Type const& ref(Dim... index) const {
        assert(((index < Dimensions) && ...));
        return *cell(index...);
    }

    template<std::convertible_to<size_t>... Dim>
    Type get(Dim... index) const {
        if (!m_storage) {
            // Array doesn't take memory but appears default-initialized here.
            return T {};
        }
        assert(((index < Dimensions) && ...));
        return *cell(index...);
    }

    template<std::convertible_to<size_t>... Dim>
    void set(Dim... index, T value) {
        assert(((index < Dimensions) && ...));
        *cell(index...) = std::move(value);
    }

    template<class Callback>
    requires(std::is_invocable_v<Callback, Vector, T&>) void for_each_cell(Callback&& callback) {
        for (size_t s = 0; s < size(); s++) {
            callback(index_to_coords(s), m_storage[s]);
        }
    }

    template<class Callback>
    requires(std::is_invocable_v<Callback, Vector, T const&>) void for_each_cell(Callback&& callback) const {
        for (size_t s = 0; s < size(); s++) {
            callback(index_to_coords(s), m_storage[s]);
        }
    }

private:
    template<std::convertible_to<size_t>... Dim>
    Type* cell(Dim... index) { return &m_storage[coords_to_index(index...)]; }

    template<std::convertible_to<size_t>... Dim>
    Type const* cell(Dim... index) const { return &m_storage[coords_to_index(index...)]; }

    template<std::convertible_to<size_t>... Dim>
    constexpr size_t coords_to_index(Dim... index) const {
        size_t idx = 0;
        ((idx *= Dimensions, idx += index), ...);
        return idx;
    }

    constexpr Vector index_to_coords(size_t index) const {
        return index_to_coords_impl(index, std::make_index_sequence<Rank>());
    }

    template<size_t... Seq>
    constexpr Vector index_to_coords_impl(size_t index, std::index_sequence<Seq...>) const {
        Vector out;
        ((out.components[Seq] = index % Dimensions, index /= Dimensions), ...);
        return out;
    }

    Type* m_storage = nullptr;
};

}
