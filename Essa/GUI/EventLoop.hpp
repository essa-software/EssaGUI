#pragma once
#include <functional>

namespace GUI {

// This is not a real event loop, this isn't abstracted enough etc
class EventLoop {
public:
    virtual ~EventLoop() = default;

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

private:
    virtual void tick() = 0;

    bool m_running = true;
    float m_tps = 0;
    int m_tps_limit = 0;
};

}
