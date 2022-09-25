#include "Window.hpp"

#include <LLGL/OpenGL/Error.hpp>

namespace GUI {

Window::Window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& settings)
    : llgl::Window { size, title, settings } {
    llgl::opengl::enable_debug_output();
}

void Window::clear(Util::Color color) {
    set_active();
    renderer().clear(color);
}

}
