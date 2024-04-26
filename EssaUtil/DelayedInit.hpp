#pragma once

#include <cassert>
#include <fmt/format.h>
#include <type_traits>
#include <utility>

namespace Util {

template<class T>
struct DependentPtr {
    using Type = T*;
};
template<class T>
struct DependentPtr<T&> {
    using Type = T*;
};

template<class T>
class DelayedInit {
public:
    static constexpr bool IsReference = std::is_reference_v<T>;
    using StorageType = std::conditional_t<IsReference, std::remove_reference_t<T>*, T>;
    using PtrType = DependentPtr<T>::Type;
    using ConstPtrType = DependentPtr<std::remove_const_t<T> const>::Type;
    using RefType = std::remove_reference_t<T>&;
    using ConstRefType = std::remove_const_t<std::remove_reference_t<T>> const&;

    DelayedInit() = default;

    // TODO: Implement these
    DelayedInit(DelayedInit const&) = delete;
    DelayedInit& operator=(DelayedInit const&) = delete;
    DelayedInit(DelayedInit&&) = delete;
    DelayedInit& operator=(DelayedInit&&) = delete;

    template<class... Args>
    DelayedInit(Args&&... object)
        requires(!IsReference)
    {
        construct(std::forward<Args>(object)...);
    }
    DelayedInit(RefType rt)
        requires(IsReference)
    {
        construct(std::forward<RefType>(rt));
    }
    DelayedInit(std::remove_reference<T>&& rt)
        requires(IsReference)
    = delete;

    template<class... Args>
    DelayedInit& operator=(Args&&... object)
        requires(!IsReference)
    {
        construct(std::forward<Args>(object)...);
        return *this;
    }
    DelayedInit& operator=(RefType rt)
        requires(IsReference)
    {
        construct(std::forward<RefType>(rt));
        return *this;
    }
    DelayedInit& operator=(std::remove_reference<T>&& rt)
        requires(IsReference)
    = delete;

    ~DelayedInit() {
        if (m_initialized)
            destruct();
    }

    template<class... Args>
    void construct(Args&&... args)
        requires(!IsReference)
    {
        if (m_initialized)
            destruct();
        new (m_storage) StorageType { std::forward<Args>(args)... };
        m_initialized = true;
    }

    void construct(RefType rt)
        requires(IsReference)
    {
        if (m_initialized)
            destruct();
        new (m_storage) StorageType { &rt };
        m_initialized = true;
    }
    void construct(std::remove_reference<T>&& rt)
        requires(IsReference)
    = delete;

    void destruct() {
        assert(m_initialized);
        if constexpr (!IsReference) {
            ptr()->~T();
        }
        m_initialized = false;
    }

    PtrType ptr() {
        if (!m_initialized)
            return nullptr;
        if constexpr (IsReference) {
            return std::bit_cast<PtrType>(m_storage);
        }
        else {
            return reinterpret_cast<PtrType>(&m_storage);
        }
    }
    ConstPtrType ptr() const {
        if (!m_initialized)
            return nullptr;
        if constexpr (IsReference) {
            return std::bit_cast<ConstPtrType>(m_storage);
        }
        else {
            return reinterpret_cast<ConstPtrType>(&m_storage);
        }
    }

    PtrType operator->() {
        assert(m_initialized);
        return ptr();
    }
    ConstPtrType operator->() const {
        assert(m_initialized);
        return ptr();
    }
    RefType operator*() {
        assert(m_initialized);
        return *ptr();
    }
    ConstRefType operator*() const {
        assert(m_initialized);
        return *ptr();
    }

    bool is_initialized() const {
        return m_initialized;
    }

private:
    bool m_initialized { false };
    alignas(StorageType) char m_storage[sizeof(StorageType)];
};

}

template<class T>
class fmt::formatter<Util::DelayedInit<T>> : public fmt::formatter<std::remove_cvref_t<T>> {
public:
    auto format(Util::DelayedInit<T> const& obj, fmt::format_context& ctx) const {
        if (!obj.is_initialized()) {
            return fmt::format_to(ctx.out(), "<not-initialized>");
        }
        return fmt::formatter<std::remove_cvref_t<T>>::format(*obj, ctx);
    }
};
