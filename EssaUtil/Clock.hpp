#pragma once

#include <chrono>

namespace Util {

class Clock {
public:
    static std::chrono::system_clock::time_point now() { return std::chrono::system_clock::now(); }

    auto elapsed() const { return now() - m_start; }
    auto restart()
    {
        auto elapsed = this->elapsed();
        m_start = now();
        return elapsed;
    }

private:
    std::chrono::system_clock::time_point m_start = now();
};

}
