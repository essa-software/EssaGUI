#include "EventLoop.hpp"

#include <SFML/System.hpp>

namespace GUI {

void EventLoop::run() {
    sf::Clock fps_clock;
    while (m_running) {
        tick();
        m_fps = 1.f / fps_clock.restart().asSeconds();
    }
}

}
