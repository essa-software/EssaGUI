#include "Window.hpp"

#include "Impls/SDLWindow.hpp"

#include <fmt/format.h>
#include <iostream>

namespace llgl {

// This must be here because otherwise we fail to compile because
// of incomplete SDLWindowData
Window::Window() = default;

Window::Window(Util::Size2u size, Util::UString const& title, WindowSettings const& settings) {
    create(size, title, settings);
}

Window Window::create_foreign(std::unique_ptr<Detail::SDLWindowData> data) {
    Window window;
    window.m_foreign = true;
    window.m_data = std::move(data);
    return window;
}

Window::~Window() {
    close();
}

void Window::create(Util::Size2u size, Util::UString const& title, WindowSettings const& settings) {
    m_size = size;
    m_renderer.m_size = m_size;
    create_impl(size, std::move(title), settings);
}

void Window::set_size(Util::Size2u size) {
    m_size = size;
    m_renderer.m_size = m_size;
    set_size_impl(size);
}

std::optional<Event> Window::poll_event() {
    auto event = poll_event_impl();
    if (event) {
        if (auto* resize = event->get<Event::WindowResize>()) {
            m_size = resize->new_size();
            m_renderer.m_size = m_size;
        }
    }
    return event;
}

void Window::center_on_screen() {
    auto screen_size = this->screen_size();
    set_position({ screen_size.x() / 2 - size().x() / 2, screen_size.y() / 2 - size().y() / 2 });
}

bool Window::is_closed() const {
    // TODO: Impl for foreign windows
    return !m_data;
}

void Window::close() {
    if (m_captures_mouse) {
        capture_mouse(false);
    }
    if (!m_foreign) {
        destroy();
    }
}

}
