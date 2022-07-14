#include "EventLoop.hpp"

namespace GUI {

void EventLoop::run() {
    // sf::Clock fps_clock;
    while (m_running) {
        tick();
        // TODO: Clock
        // m_fps = 1.f / fps_clock.restart().asSeconds();
    }
}

}
