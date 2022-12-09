#include "XWindow.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

namespace llgl
{

XWindow::~XWindow()
{
    close();
    if(m_display)
        XCloseDisplay(m_display);
}

void XWindow::set_title(std::string const& title)
{
    m_title = title;
}

void XWindow::set_size(Size size)
{
    m_size = size;
}

void XWindow::open()
{
    close();
    m_display = XOpenDisplay(nullptr);
    if(!m_display)
    {
        std::cout << "Failed to open display" << std::endl;
        return;
    }
    Visual* visual = DefaultVisual(m_display, 0);
    if(!visual)
    {
        std::cout << "Failed to get visual" << std::endl;
        return;
    }
    XVisualInfo visual_template;
    visual_template.screen = DefaultScreen(m_display);
    int visual_count = 0;
    auto visual_info = XGetVisualInfo(m_display, VisualScreenMask, &visual_template, &visual_count);
    auto colormap = XCreateColormap(m_display, RootWindow(m_display, visual_info->screen), visual_info->visual, AllocNone);

    /* create a window */
    XSetWindowAttributes attributes;
    attributes.colormap = colormap;
    attributes.border_pixel = 0;
    attributes.event_mask =  ExposureMask | KeyPressMask | ButtonPress |
                              StructureNotifyMask | ButtonReleaseMask |
                              KeyReleaseMask | EnterWindowMask | LeaveWindowMask |
                              PointerMotionMask | Button1MotionMask | VisibilityChangeMask |
                              ColormapChangeMask;
    m_window = XCreateWindow(m_display, DefaultRootWindow(m_display), 0, 0, m_size.x, m_size.y, 1, 32,
        CopyFromParent, visual, CWEventMask | CWBackPixel | CWBorderPixel, &attributes);
    if(!m_window)
    {
        std::cout << "Failed to create window" << std::endl;
        return;
    }
    if(!XStoreName(m_display, m_window, m_title.c_str()))
    {
        std::cout << "Failed to set window's title" << std::endl;
        return;
    }
    if(!XMapWindow(m_display, m_window))
    {
        std::cout << "Failed to map window" << std::endl;
        return;
    }
}

void XWindow::close()
{
    if(m_window)
        XDestroyWindow(m_display, m_window);
}

}
