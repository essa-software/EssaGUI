#pragma once

#include <EssaGUI/gui/WidgetTreeRoot.hpp>
#include <functional>

namespace GUI {

class HostWindow;

class Overlay : public WidgetTreeRoot {
public:
    Overlay(HostWindow& window, std::string id)
        : m_host_window(window)
        , m_id(std::move(id)) { }

    void close() {
        m_closed = true;
        quit();
    }
    bool is_closed() const { return m_closed; }

    std::string id() const { return m_id; }

    HostWindow& host_window() { return m_host_window; }
    HostWindow const& host_window() const { return m_host_window; }

    std::function<void()> on_close;

private:
    HostWindow& m_host_window;
    std::string m_id;
    bool m_closed = false;
};

}
