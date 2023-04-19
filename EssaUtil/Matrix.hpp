#pragma once

#include "CoordinateSystem.hpp"
#include "Vector.hpp"
#include <cassert>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iostream>

#ifdef ESSA_COMPILER_GCC
#    pragma GCC optimize("O3")
#endif

namespace Util {

template<class T, size_t Size>
    requires(Size > 0)
struct Matrix {

    constexpr Matrix() {
        for (size_t s = 0; s < Size; s++)
            m_data[s][s] = 1;
    }

    constexpr Matrix(std::initializer_list<T> data) {
        assert(data.size() == Size * Size);
        for (size_t row = 0; row < Size; row++) {
            for (size_t column = 0; column < Size; column++) {
                m_data[row][column] = data.begin()[row * Size + column];
            }
        }
    }

    template<class... T2>
    constexpr Matrix(T2... data)
        requires(sizeof...(data) == Size * Size)
        : Matrix { std::initializer_list<T> { data... } } {
    }

    static constexpr Matrix identity() {
        return {};
    }

    T& element(size_t row, size_t column) { return m_data[row][column]; }
    T const& element(size_t row, size_t column) const { return m_data[row][column]; }

    Matrix transposed() const;
    Matrix inverted() const;
    T determinant() const;
    Matrix adjoint() const;
    Matrix<T, Size - 1> minor(size_t row, size_t column) const;

    template<class TT>
    Matrix<TT, Size> convert() const {
        Matrix<TT, Size> output;
        for (size_t x = 0; x < Size; x++) {
            for (size_t y = 0; y < Size; y++) {
                output.element(x, y) = static_cast<T>(element(x, y));
            }
        }
        return output;
    }

    // This must be transposed to pass this to OpenGL
    // FIXME: Fix that.
    T const* raw_data() const { return &m_data[0][0]; }

    constexpr bool operator==(Matrix<T, Size> const&) const = default;

private:
    T m_data[Size][Size] {};
};

template<class T, size_t Size>
std::ostream& operator<<(std::ostream& out, Matrix<T, Size> const& mat) {
    out << "[";
    for (size_t x = 0; x < Size; x++) {
        for (size_t y = 0; y < Size; y++) {
            out << mat.element(x, y);
            if (y != Size - 1)
                out << " ";
        }
        if (x != Size - 1)
            out << "; ";
    }
    out << "]";
    return out;
}

using Matrix4x4f = Matrix<float, 4>;
using Matrix4x4d = Matrix<double, 4>;

template<class T, size_t Size>
Matrix<T, Size> operator*(Matrix<T, Size> const& left, T right) {
    Matrix<T, Size> result;
    for (size_t i = 0; i < Size; i++) {
        for (size_t j = 0; j < Size; j++) {
            result.element(i, j) = left.element(i, j) * right;
        }
    }
    return result;
}
template<class T, size_t Size>
Matrix<T, Size> operator*(T left, Matrix<T, Size> const& right) {
    return right * left;
}

template<class T, size_t Size>
Matrix<T, Size> operator*(Matrix<T, Size> const& left, Matrix<T, Size> const& right) {
    // FIXME: I am naive
    Matrix<T, Size> result;
    for (size_t i = 0; i < Size; i++) {
        for (size_t j = 0; j < Size; j++) {
            T sum = 0;
            for (size_t k = 0; k < Size; k++)
                sum += left.element(i, k) * right.element(k, j);
            result.element(i, j) = sum;
        }
    }
    return result;
}

template<class T, size_t Size>
    requires(Size > 0)
inline Matrix<T, Size> Matrix<T, Size>::inverted() const {
    // https://math.icalculator.info/4x4-matrix-inverse-calculator.html
    // A^-1 = 1 / det(A) × adj(A)
    return 1 / determinant() * adjoint();
}

template<class T, size_t Size>
    requires(Size > 0)
Matrix<T, Size - 1> inline Matrix<T, Size>::minor(size_t row, size_t column) const {
    Matrix<T, Size - 1> submatrix;
    size_t source_i = 0;
    for (size_t ii = 0; ii < Size - 1; ii++) {
        size_t source_j = 0;
        if (source_i == row)
            source_i++;
        for (size_t jj = 0; jj < Size - 1; jj++) {
            if (source_j == column)
                source_j++;
            submatrix.element(ii, jj) = element(source_i, source_j);
            source_j++;
        }
        source_i++;
    }
    return submatrix;
}

template<class T, size_t Size>
    requires(Size > 0)
inline T Matrix<T, Size>::determinant() const {
    if constexpr (Size == 1)
        return m_data[0][0];
    if constexpr (Size == 2)
        return m_data[0][0] * m_data[1][1] - m_data[0][1] * m_data[1][0];

    if constexpr (Size > 2) {
        // N > 2 - Laplace Expansion
        // det A = sum(n, i=1) -1^(i+j) * a_ij det A_ij
        // assume j = 0
        T det {};
        for (size_t i = 0; i < Size; i++) {
            T sign = i % 2 == 0 ? 1 : -1;
            Matrix<T, Size - 1> submatrix = minor(i, 0);
            T subdet = submatrix.determinant();
            det += sign * element(i, 0) * subdet;
        }
        return det;
    }
}

template<class T, size_t Size>
    requires(Size > 0)
inline Matrix<T, Size> Matrix<T, Size>::transposed() const {
    Matrix result;
    for (size_t i = 0; i < Size; i++) {
        for (size_t j = 0; j < Size; j++) {
            result.element(j, i) = element(i, j);
        }
    }
    return result;
}

template<class T, size_t Size>
    requires(Size > 0)
inline Matrix<T, Size> Matrix<T, Size>::adjoint() const {
    Matrix cofactors;

    for (size_t i = 0; i < Size; i++) {
        for (size_t j = 0; j < Size; j++) {
            cofactors.element(i, j) = ((i + j) % 2 == 0 ? 1 : -1) * minor(i, j).determinant();
        }
    }

    return cofactors.transposed();
}

template<class T>
inline Cs::Point4<T> operator*(Matrix<T, 4> const& mat, Cs::Point4<T> const& vec) {
    Cs::Point4<T> result;
    result.set_x(vec.x() * mat.element(0, 0) + vec.y() * mat.element(0, 1) + vec.z() * mat.element(0, 2) + vec.w() * mat.element(0, 3));
    result.set_y(vec.x() * mat.element(1, 0) + vec.y() * mat.element(1, 1) + vec.z() * mat.element(1, 2) + vec.w() * mat.element(1, 3));
    result.set_z(vec.x() * mat.element(2, 0) + vec.y() * mat.element(2, 1) + vec.z() * mat.element(2, 2) + vec.w() * mat.element(2, 3));
    result.set_w(vec.x() * mat.element(3, 0) + vec.y() * mat.element(3, 1) + vec.z() * mat.element(3, 2) + vec.w() * mat.element(3, 3));
    return result;
}

}
