#pragma once

#include <cassert>
#include <chrono>
#include <functional>

namespace GUI {

class Timer {
    friend class EventLoop;

    using Callback = std::function<void()>;
    using Clock = std::chrono::system_clock;

    enum class Mode {
        SingleShot, // Will fire once after `timeout` and finish
        MultiShot   // Will fire every `timeout` seconds
    };

    Timer(Clock::duration const& timeout, Callback callback, Mode mode)
        : m_timeout(timeout)
        , m_callback(std::move(callback))
        , m_mode(mode) {
        m_last_run = Clock::now();
        assert(m_callback);
    }

    void update();
    void reset();

    // Returns true if timer won't fire again and can be removed.
    bool finished() const { return m_finished; }

    Clock::duration m_timeout;
    Callback m_callback;
    Mode m_mode;

    bool m_finished = false;
    Clock::time_point m_last_run;
};

}
