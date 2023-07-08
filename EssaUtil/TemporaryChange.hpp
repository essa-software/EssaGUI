#pragma once

#include <utility>

namespace Util {

template<class T> class TemporaryChange {
public:
    TemporaryChange(T& variable, T&& new_value)
        : m_variable(variable)
        , m_old_value(std::move(m_variable)) {
        m_variable = std::move(new_value);
    }

    ~TemporaryChange() { m_variable = std::move(m_old_value); }

private:
    T& m_variable;
    T m_old_value;
};

}
