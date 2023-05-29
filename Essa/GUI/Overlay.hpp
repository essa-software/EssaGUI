#pragma once

#include <Essa/GUI/WidgetTreeRoot.hpp>
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

    virtual Util::Point2i position() const override { return m_position; }
    void set_position(Util::Point2i position) { m_position = position; }
    virtual Util::Size2i size() const override { return m_size; }
    void set_size(Util::Size2i size) { m_size = size; }

    HostWindow& host_window() { return m_host_window; }
    HostWindow const& host_window() const { return m_host_window; }

    std::function<void()> on_close;

    // If true, window will not receive any events except Global events, or if explicitly focused.
    void set_ignores_events(bool pass) { m_ignore_events = pass; }
    bool ignores_events() const { return m_ignore_events; }

    // If true, focusing a window won't bring it to the top.
    void set_always_on_bottom(bool value) { m_always_on_bottom = value; }
    bool always_on_bottom() const { return m_always_on_bottom; }

private:
    Util::Point2i m_position;
    Util::Size2i m_size;
    HostWindow& m_host_window;
    std::string m_id;
    bool m_closed = false;
    bool m_ignore_events = false;
    bool m_always_on_bottom = false;
};

}
