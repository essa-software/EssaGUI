#pragma once

#include <source_location>

namespace llgl::opengl {

void enable_debug_output();

void handle_error(std::source_location = std::source_location::current());
class ErrorHandler {
public:
    ErrorHandler(std::source_location location = std::source_location::current())
        : m_location(location) { }

    ~ErrorHandler() {
        handle_error(m_location);
    }

private:
    std::source_location m_location;
};

}
