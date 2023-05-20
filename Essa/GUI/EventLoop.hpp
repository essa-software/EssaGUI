#pragma once

#include <Essa/GUI/Timer.hpp>

#include <functional>
#include <list>
#include <memory>
#include <set>

namespace GUI {

// This is not a real event loop, this isn't abstracted enough etc
class EventLoop {
public:
    virtual ~EventLoop() = default;

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
    TimerHandle set_interval(Timer::Clock::duration const&, Timer::Callback&&);
    void remove_timer(TimerHandle);
    static void reset_timer(TimerHandle);

private:
    virtual void tick() = 0;

    bool m_running = true;
    float m_tps = 0;
    int m_tps_limit = 0;
    std::set<std::shared_ptr<Timer>> m_timers;
};

}
