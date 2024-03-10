#pragma once

#include <Essa/GUI/Timer.hpp>

#include <functional>
#include <memory>
#include <set>

namespace GUI {

class TimerScopeGuard;

// This is not a real event loop, this isn't abstracted enough etc
class EventLoop {
public:
    virtual ~EventLoop() = default;

    static bool has_current();
    static EventLoop& current();

    std::function<bool()> on_close;

    void run();
    void quit() {
        if (on_close)
            m_running = !on_close();
        else
            m_running = false;
    }
    float tps() const { return m_tps; }

    bool is_running() const { return m_running; }

    void set_tps_limit(int l) { m_tps_limit = l; }

    using TimerHandle = std::weak_ptr<Timer>;

    TimerHandle set_timeout(Timer::Clock::duration const&, Timer::Callback&&);
    [[nodiscard]] TimerScopeGuard set_timeout_with_guard(Timer::Clock::duration const&, Timer::Callback&&);
    TimerHandle set_interval(Timer::Clock::duration const&, Timer::Callback&&);
    [[nodiscard]] TimerScopeGuard set_interval_with_guard(Timer::Clock::duration const&, Timer::Callback&&);
    void remove_timer(TimerHandle const&);
    static void reset_timer(TimerHandle const&);

    template<class Callback> void deferred_invoke(Callback&& c) {
        using namespace std::chrono_literals;
        set_timeout(0s, [c = std::forward<Callback>(c)]() { c(); });
    }

private:
    virtual void tick() = 0;

    bool m_running = true;
    float m_tps = 0;
    int m_tps_limit = 0;
    std::set<std::shared_ptr<Timer>> m_timers;
};

// A class that removes the given Timer after destruction
class TimerScopeGuard {
public:
    TimerScopeGuard(EventLoop& loop, std::weak_ptr<Timer> timer)
        : m_loop(loop)
        , m_timer(std::move(timer)) { }
    ~TimerScopeGuard() {
        if (!m_timer.expired()) {
            m_loop.remove_timer(m_timer);
        }
    }

    TimerScopeGuard(TimerScopeGuard const&) = delete;
    TimerScopeGuard& operator=(TimerScopeGuard const&) = delete;
    TimerScopeGuard(TimerScopeGuard&&) = default;

private:
    EventLoop& m_loop;
    std::weak_ptr<Timer> m_timer;
};

}
