#pragma once

#include <Essa/LLGL/Core/Size.hpp>

#include <X11/Xlib.h>
#include <string>

namespace llgl {

class XWindow {
public:
    ~XWindow();
    void set_title(std::string const&);
    void set_size(Size);
    void open();
    void close();

private:
    ::Display* m_display;
    ::Window m_window;
    std::string m_title;
    Size m_size;
};

}
