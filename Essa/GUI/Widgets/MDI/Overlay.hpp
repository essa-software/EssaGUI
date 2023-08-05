#pragma once

#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <functional>

namespace GUI::MDI {

class Host;

// FIXME: Get rid of this when all "special" overlays become native
class Overlay : public WidgetTreeRoot {
public:
    Overlay(Host& host, std::string id);

    virtual void close() override { m_closed = true; }
    bool is_closed() const { return m_closed; }

    std::string id() const { return m_id; }

    virtual void setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const&) override;
    virtual void center_on_screen() override;

    virtual Util::Point2i position() const override { return m_position; }
    void set_position(Util::Point2i position) { m_position = position; }
    virtual Util::Size2i size() const override { return m_size; }
    virtual void set_size(Util::Size2i size) override { m_size = size; }

    Host& host() { return m_host; }
    Host const& host() const { return m_host; }
    HostWindow& host_window() { return const_cast<HostWindow&>(static_cast<Overlay const&>(*this).host_window()); }
    HostWindow const& host_window() const;

    std::function<void()> on_close;

    // If true, window will not receive any events except Global events, or if explicitly focused.
    void set_ignores_events(bool pass) { m_ignore_events = pass; }
    bool ignores_events() const { return m_ignore_events; }

    // If true, focusing a window won't bring it to the top.
    void set_always_on_bottom(bool value) { m_always_on_bottom = value; }
    bool always_on_bottom() const { return m_always_on_bottom; }

protected:
    friend class Host;

private:
    Util::Point2i m_position;
    Util::Size2i m_size;
    Host& m_host;
    std::string m_id;
    bool m_closed = false;
    bool m_ignore_events = false;
    bool m_always_on_bottom = false;
};

}
