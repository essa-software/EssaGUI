#include "EventLoop.hpp"

#include <EssaUtil/Clock.hpp>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace GUI {

void EventLoop::run() {
    Util::Clock clock;
    while (m_running) {
        auto tick_start = std::chrono::system_clock::now();
        tick();
        m_tps = 1.f / (clock.restart() / 1.0s);
        if (m_tps_limit > 0) {
            auto const expected_tick_time = 1.0s / m_tps_limit;
            std::this_thread::sleep_until(tick_start + expected_tick_time);
        }
    }
}

}
