#pragma once

namespace Util {

template<class Callback>
requires requires(Callback c) { c(); }
class ScopeGuard {
public:
    ScopeGuard(Callback&& c)
        : m_callback(std::move(c)) { }

    ~ScopeGuard() { m_callback(); }

private:
    Callback m_callback;
};

}
