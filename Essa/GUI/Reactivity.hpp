#pragma once

#include <EssaUtil/Config.hpp>
#include <cassert>
#include <concepts>
#include <functional>
#include <map>
#include <type_traits>
#include <variant>

namespace GUI {

template<std::copyable T> class Observer;

namespace Reactivity {

template<class Observable, class Mapper> class Map {
public:
    using Type = std::invoke_result_t<Mapper, typename Observable::Type>;
    using ObserverFunc = std::function<void(Type const&)>;

    Map(Observable obs, Mapper mapper)
        : m_observable(std::move(obs))
        , m_mapper(std::move(mapper)) { }

    void observe(ObserverFunc&& observer) {
        return m_observable.observe([this, observer = std::move(observer)](auto const& t) { observer(m_mapper(t)); });
    }

private:
    Observable m_observable;
    Mapper m_mapper;
};
}

template<std::copyable T> class ObservableBase {
public:
    using Type = T;

    ObservableBase() = default;
    ObservableBase(ObservableBase const&) = delete;
    ObservableBase(ObservableBase&&) = delete;
    virtual ~ObservableBase() = default;

    using ObserverFunc = std::function<void(T const&)>;

    void observe(ObserverFunc&& observer) const {
        assert(observer);
        // Inform about the initial value
        observer(get());
        m_observers.push_back(std::move(observer));
    }

    virtual T get() const = 0;

    template<class Mapper> auto map(Mapper mapper) const { return Reactivity::Map<decltype(*this), Mapper> { *this, std::move(mapper) }; }

protected:
    void notify(T const& state) {
        for (auto const& obs : m_observers) {
            obs(state);
        }
    }

private:
    mutable std::vector<ObserverFunc> m_observers;
};

// Observable with constant value
template<std::copyable T> class Observable : public ObservableBase<T> {
public:
    Observable(T&& t)
        : m_value(std::forward<T>(t)) { }

    Observable& operator=(T const& t) {
        m_value = t;
        this->notify(t);
        return *this;
    }

    virtual T get() const { return m_value; }

private:
    T m_value;
};

// Observable that calculates its value on the fly. They are
// typically exposed with ReadOnlyObservable.
template<std::copyable T> class CalculatedObservable : public ObservableBase<T> {
public:
    using Calculator = std::function<T()>;

    CalculatedObservable(Calculator&& t)
        : m_calculator(std::move(t)) {
        assert(m_calculator);
    }

    virtual T get() const override { return m_calculator(); }
    using ObservableBase<T>::notify;

private:
    Calculator m_calculator;
};

// Read-only wrapper for Observable. Expose this if you want to make
// some field public but read only
template<std::copyable T> class ReadOnlyObservable {
public:
    using Type = T;
    using ObserverFunc = std::function<void(T const&)>;

    ReadOnlyObservable(ObservableBase<T> const& ob)
        : m_observable(ob) { }

    void observe(ObserverFunc&& observer) const { m_observable.observe(std::move(observer)); }
    T const& get() const { return m_observable.m_value; }

    template<class Mapper> auto map(Mapper mapper) const {
        return Reactivity::Map<std::remove_reference_t<decltype(*this)>, Mapper> {
            *this,
            std::move(mapper),
        };
    }

private:
    ObservableBase<T> const& m_observable;
};

// Value that is read from somewhere. It may contain a function that
// updates its value on every read.
template<std::copyable T> class Read {
public:
    using Reader = std::function<T()>;

    Read() = default;

    // Construct from a function that will compute the value
    // for every read.
    Read& operator=(Reader&& t) {
        assert(t);
        // TODO: Deregister observer?
        assert(!m_observing);
        m_value = std::move(t);
        return *this;
    }

    Read& operator=(T&& t) {
        // TODO: Deregister observer?
        assert(!m_observing);
        m_value = std::move(t);
        return *this;
    }

    Read(Read const&) = delete;
    Read(Read&&) = delete;

    // If possible, use lvalue ref
    Read& operator=(ObservableBase<T> const& obs) {
        // Only a single source is allowed for read
        assert(!m_observing);
        obs.observe([this](auto v) { m_value = std::move(v); });
        m_observing = true;
        return *this;
    }

    // Wrappers (like ReadOnlyObservable or Map) may be lvalue
    Read& operator=(auto&& obs)
        requires requires() { obs.observe([](std::remove_reference_t<decltype(obs)>::Type) {}); }
    {
        // Only a single source is allowed for read
        assert(!m_observing);
        obs.observe([this](std::remove_reference_t<decltype(obs)>::Type v) { m_value = std::move(v); });
        m_observing = true;
        return *this;
    }

    T get() const {
        return std::visit(
            Util::Overloaded {
                [&](std::monostate) -> T { ESSA_UNREACHABLE; },
                [&](Reader const& v) -> T { return v(); },
                [&](T const& t) -> T { return t; },
            },
            m_value
        );
    }

private:
    std::variant<std::monostate, T, Reader> m_value;
    bool m_observing = false;
};

}
