#pragma once

#include <cassert>
#include <utility>

namespace Util
{

template<class T>
class DelayedInit
{
public:
    DelayedInit() = default;

    // TODO: Implement these
    DelayedInit(DelayedInit const&) = delete;
    DelayedInit& operator=(DelayedInit const&) = delete;
    DelayedInit(DelayedInit&&) = delete;
    DelayedInit& operator=(DelayedInit&&) = delete;

    template<class... Args>
    DelayedInit(Args&&... object)
    {
        construct(std::forward<Args>(object)...);
    }

    ~DelayedInit()
    {
        if (m_initialized)
            destruct();
    }

    template<class... Args>
    void construct(Args&&... args)
    {
        if (m_initialized)
            destruct();
        new (m_storage) T { std::forward<Args>(args)... };
        m_initialized = true;
    }

    void destruct()
    {
        assert(m_initialized);
        ptr()->~T();
        m_initialized = false;
    }

    T* ptr()
    {
        if (!m_initialized)
            return nullptr;
        return reinterpret_cast<T*>(m_storage);
    }
    T const* ptr() const
    {
        if (!m_initialized)
            return nullptr;
        return reinterpret_cast<T const*>(m_storage);
    }

    T* operator->() { return ptr(); }
    T const* operator->() const { return ptr(); }
    T& operator*()
    {
        assert(m_initialized);
        return *ptr();
    }
    T const& operator*() const
    {
        assert(m_initialized);
        return *ptr();
    }

    bool is_initialized() const { return m_initialized; }

private:
    bool m_initialized { false };
    alignas(T) char m_storage[sizeof(T)];
};

}
