#include "Timer.hpp"

#include <fmt/chrono.h>

namespace GUI {

void Timer::update() {
    auto current_time = Clock::now();
    if (current_time - m_last_run >= m_timeout) {
        m_last_run = current_time;
        m_callback();
        if (m_mode == Mode::SingleShot) {
            m_finished = true;
        }
    }
}

void Timer::reset() { m_last_run = Clock::now(); }

}
