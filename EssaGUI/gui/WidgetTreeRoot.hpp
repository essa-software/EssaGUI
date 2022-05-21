#pragma once

#include "EventLoop.hpp"
#include "Widget.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
#include <memory>

namespace GUI {

class WidgetTreeRoot : public EventLoop {
public:
    explicit WidgetTreeRoot(sf::RenderWindow& wnd)
        : m_window(wnd) { }

    WidgetTreeRoot(WidgetTreeRoot const&) = delete;
    WidgetTreeRoot& operator=(WidgetTreeRoot const&) = delete;

    virtual ~WidgetTreeRoot() = default;

    sf::RenderWindow& window() const { return m_window; }
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

    void set_id(std::string id){m_id = id;}

    virtual void draw();
    virtual void handle_event(sf::Event);
    virtual void handle_events() {}
    virtual void update() { m_main_widget->do_update(); }

    virtual sf::Vector2f position() const = 0;
    virtual sf::Vector2f size() const = 0;
    sf::FloatRect rect() const { return { position(), size() }; }

    // The rect that the WidgetTreeRoot should consume events from. For
    // ToolWindows, it is content + titlebar.
    virtual sf::FloatRect full_rect() const { return rect(); }

protected:
    virtual void tick() override;

    bool pass_event_to_window_if_needed(WidgetTreeRoot& wtr, sf::Event event);
    sf::Event transform_event(sf::Vector2f offset, sf::Event event) const;

private:
    sf::RenderWindow& m_window;
    Widget* m_focused_widget {};
    bool m_needs_relayout = true;
    std::shared_ptr<Widget> m_main_widget;

    std::string m_id = "";
};

}
