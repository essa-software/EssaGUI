#include "EventLoop.hpp"

#include <EssaUtil/Clock.hpp>

using namespace std::chrono_literals;

namespace GUI {

void EventLoop::run() {
    Util::Clock clock;
    while (m_running) {
        tick();
        m_tps = 1.f / (clock.restart() / 1.0s);
    }
}

}
