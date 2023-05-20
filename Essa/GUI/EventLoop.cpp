#include "EventLoop.hpp"

#include <Essa/GUI/Timer.hpp>
#include <EssaUtil/Clock.hpp>
#include <EssaUtil/ScopeGuard.hpp>
#include <cassert>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace GUI {

static EventLoop* s_current_event_loop = nullptr;

EventLoop& EventLoop::current() {
    assert(s_current_event_loop);
    return *s_current_event_loop;
}

void EventLoop::run() {
    auto old_event_loop = s_current_event_loop;
    s_current_event_loop = this;
    Util::ScopeGuard guard = [&]() { s_current_event_loop = old_event_loop; };

    Util::Clock clock;
    while (m_running) {
        auto tick_start = std::chrono::system_clock::now();

        for (auto& timer : m_timers) {
            timer->update();
        }
        std::erase_if(m_timers, [](auto const& timer) { return timer->finished(); });

        tick();

        m_tps = 1.f / (clock.restart() / 1.0s);
        if (m_tps_limit > 0) {
            auto const expected_tick_time = 1.0s / m_tps_limit;
            std::this_thread::sleep_until(tick_start + expected_tick_time);
        }
    }
}

EventLoop::TimerHandle EventLoop::set_timeout(Timer::Clock::duration const& timeout, Timer::Callback&& callback) {
    return *m_timers.insert(std::shared_ptr<Timer>(new Timer(timeout, std::move(callback), Timer::Mode::SingleShot))).first;
}

EventLoop::TimerHandle EventLoop::set_interval(Timer::Clock::duration const& timeout, Timer::Callback&& callback) {
    return *m_timers.insert(std::shared_ptr<Timer>(new Timer(timeout, std::move(callback), Timer::Mode::MultiShot))).first;
}

void EventLoop::remove_timer(TimerHandle handle) { m_timers.erase(handle.lock()); }

void EventLoop::reset_timer(TimerHandle handle) {
    if (auto timer = handle.lock()) {
        timer->reset();
    }
}

}
