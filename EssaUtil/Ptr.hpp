#pragma once

namespace Util {

template<class T>
class Ptr {
public:
    Ptr(T const& t)
        : m_ptr(&t) { }

    Ptr(T const* t)
        : m_ptr(t) { assert(t); }

    T const* operator->() const { return m_ptr; }
    T const& operator*() const { return *m_ptr; }

    T const* ptr() const { return m_ptr; }

private:
    T const* m_ptr;
};

template<class T>
class MutPtr {
public:
    MutPtr(T& t)
        : m_ptr(&t) { }

    MutPtr(T* t)
        : m_ptr(t) { assert(t); }

    T* operator->() const { return m_ptr; }
    T& operator*() const { return *m_ptr; }

    T* ptr() const { return m_ptr; }

private:
    T* m_ptr;
};

}
