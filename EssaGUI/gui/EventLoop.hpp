#pragma once

namespace GUI {

// This is not a real event loop, this isn't abstracted enough etc
class EventLoop {
public:
    virtual ~EventLoop() = default;

    void run();
    void quit() { m_running = false; }

    bool is_running() const { return m_running; }

private:
    virtual void tick() = 0;

    bool m_running = true;
    float m_fps = 0;
};

}
