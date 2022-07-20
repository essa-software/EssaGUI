#pragma once

#include "EssaGUI/gfx/ResourceManager.hpp"
#include "EventLoop.hpp"
#include "Widget.hpp"

#include <EssaGUI/gfx/Window.hpp>
#include <list>
#include <memory>

namespace GUI {

class WidgetTreeRoot : public EventLoop {
public:
    explicit WidgetTreeRoot(GUI::Window& wnd)
        : m_window(wnd) {
    }

    WidgetTreeRoot(WidgetTreeRoot const&) = delete;
    WidgetTreeRoot& operator=(WidgetTreeRoot const&) = delete;

    virtual ~WidgetTreeRoot() = default;

    GUI::Window& window() const { return m_window; }
    Widget* focused_widget() const { return m_focused_widget; }
    void set_focused_widget(Widget* w);

    void set_needs_relayout() { m_needs_relayout = true; }

    template<class T, class... Args>
    auto& set_main_widget(Args&&... args) {
        auto widget = std::make_shared<T>(*this, std::forward<Args>(args)...);
        auto widget_ptr = widget.get();
        m_main_widget = std::move(widget);
        m_needs_relayout = true;
        return *widget_ptr;
    }

    template<class T, class... Args>
    auto& set_created_main_widget(std::shared_ptr<T> w) {
        auto widget_ptr = w.get();
        m_main_widget = std::move(w);
        m_needs_relayout = true;
        return *widget_ptr;
    }

    void set_id(std::string id) { m_id = id; }

    virtual void draw();
    virtual void handle_event(llgl::Event);
    virtual void handle_events() { }
    virtual void update() {
        if (m_main_widget)
            m_main_widget->do_update();
    }

    virtual Util::Vector2f position() const = 0;
    virtual Util::Vector2f size() const = 0;
    Util::Rectf rect() const { return { position(), size() }; }

    // The rect that the WidgetTreeRoot should consume events from. For
    // ToolWindows, it is content + titlebar.
    virtual Util::Rectf full_rect() const { return rect(); }

protected:
    virtual void tick() override;

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

    bool pass_event_to_window_if_needed(WidgetTreeRoot& wtr, llgl::Event event);
    llgl::Event transform_event(Util::Vector2f offset, llgl::Event event) const;

private:
    GUI::Window& m_window;
    Widget* m_focused_widget {};
    bool m_needs_relayout = true;
    std::shared_ptr<Widget> m_main_widget;

    std::string m_id = "";
};

}
