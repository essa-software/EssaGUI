#include "EventLoop.hpp"

#include <Essa/GUI/Timer.hpp>
#include <EssaUtil/Clock.hpp>
#include <EssaUtil/ScopeGuard.hpp>

#include <cassert>
#include <chrono>
#include <fmt/chrono.h>
#include <thread>

#ifdef __linux__
#    include <sys/prctl.h>
#endif

using namespace std::chrono_literals;

namespace GUI {

static EventLoop* s_current_event_loop = nullptr;

EventLoop& EventLoop::current() {
    assert(s_current_event_loop);
    return *s_current_event_loop;
}

bool EventLoop::has_current() { return s_current_event_loop != nullptr; }

void increase_system_timer_resolution() {
    static bool increased = false;
    if (!increased) {
        increased = true;
#ifdef __linux__
        prctl(PR_SET_TIMERSLACK, 10000, 0, 0, 0);
#endif
    }
}

void EventLoop::run() {
    auto old_event_loop = s_current_event_loop;
    s_current_event_loop = this;
    Util::ScopeGuard guard = [&]() { s_current_event_loop = old_event_loop; };

    Util::Clock clock;

    increase_system_timer_resolution();

    while (m_running) {
        for (auto& timer : m_timers) {
            timer->update();
        }
        std::erase_if(m_timers, [](auto const& timer) { return timer->finished(); });

        tick();
        auto processing_time = clock.elapsed();

        if (m_tps_limit > 0) {
            auto const expected_tick_time = 1.0s / m_tps_limit;

            // System timer is not accurate enough to sleep for the exact amount of time.
            // Let's just spin while giving out individual system ticks to the OS.
            while (processing_time < expected_tick_time) {
                std::this_thread::sleep_for(0.1ms);
                processing_time = clock.elapsed();
            }
        }
        auto time = clock.restart();
        m_tps = 1.f / (time / 1.0s);
        if (m_tps_limit != 0) {
            auto lfac = 1 - (m_tps / m_tps_limit - 0.99) * 100;
            if (lfac > 1) {
                fmt::print(stderr, "[Lag] Tick took {:.2f}ms ({:.1f} TPS) LFAC_100={}\n", time / 1.0ms, m_tps, lfac);
            }
        }
    }
}

EventLoop::TimerHandle EventLoop::set_timeout(Timer::Clock::duration const& timeout, Timer::Callback&& callback) {
    return *m_timers.insert(std::shared_ptr<Timer>(new Timer(timeout, std::move(callback), Timer::Mode::SingleShot))).first;
}

TimerScopeGuard EventLoop::set_timeout_with_guard(Timer::Clock::duration const& timeout, Timer::Callback&& callback) {
    return TimerScopeGuard(*this, set_timeout(timeout, std::move(callback)));
}

EventLoop::TimerHandle EventLoop::set_interval(Timer::Clock::duration const& timeout, Timer::Callback&& callback) {
    return *m_timers.insert(std::shared_ptr<Timer>(new Timer(timeout, std::move(callback), Timer::Mode::MultiShot))).first;
}

TimerScopeGuard EventLoop::set_interval_with_guard(Timer::Clock::duration const& timeout, Timer::Callback&& callback) {
    return TimerScopeGuard(*this, set_interval(timeout, std::move(callback)));
}

void EventLoop::remove_timer(TimerHandle const& handle) { m_timers.erase(handle.lock()); }

void EventLoop::reset_timer(TimerHandle const& handle) {
    if (auto timer = handle.lock()) {
        timer->reset();
    }
}

}
