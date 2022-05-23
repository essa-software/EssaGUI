#pragma once

#include <EssaGUI/gfx/SFMLWindow.hpp>
#include <EssaGUI/gui/Theme.hpp>
#include <EssaGUI/util/Units.hpp>
#include <SFML/Graphics.hpp>
#include <cassert>
#include <string_view>

namespace GUI {

class Container;
class TooltipOverlay;
class WidgetTreeRoot;

struct LengthVector {
    Length x;
    Length y;
};

constexpr bool operator==(LengthVector const& a, LengthVector const& b) {
    return a.x == b.x && a.y == b.y;
}

class Event {
public:
    Event(sf::Event const& event)
        : m_event(event) {
    }

    sf::Event event() const { return m_event; }

    // FIXME: idk the names
    bool is_handled() const { return m_handled; }
    void set_handled() { m_handled = true; }
    bool is_seen() const { return m_seen; }
    void set_seen() { m_seen = true; }

    sf::Event::EventType type() const { return m_event.type; }

    bool is_mouse_related() const {
        return m_event.type == sf::Event::MouseMoved || m_event.type == sf::Event::MouseButtonPressed || m_event.type == sf::Event::MouseButtonReleased;
    }

    sf::Vector2f mouse_position() const {
        assert(is_mouse_related());
        if (m_event.type == sf::Event::MouseMoved)
            return { static_cast<float>(m_event.mouseMove.x), static_cast<float>(m_event.mouseMove.y) };
        if (m_event.type == sf::Event::MouseButtonPressed || m_event.type == sf::Event::MouseButtonReleased)
            return { static_cast<float>(m_event.mouseButton.x), static_cast<float>(m_event.mouseButton.y) };
        __builtin_unreachable();
    }

private:
    sf::Event m_event;
    bool m_handled = false;
    bool m_seen = false;
};

class Widget {
public:
    explicit Widget(Container& parent);

    Widget(Widget const&) = delete;
    Widget& operator=(Widget const&) = delete;

    virtual ~Widget();

    bool is_hover() const { return m_hover; }

    virtual void do_handle_event(Event& event);
    virtual void do_update();
    virtual void draw(GUI::SFMLWindow& window) const;

    void set_raw_position(sf::Vector2f p) {
        m_pos = p;
    }
    void set_raw_size(sf::Vector2f s) {
        m_size = s;
    }

    void set_position(LengthVector l) {
        m_expected_pos = l;
        set_needs_relayout();
    }

    void set_size(LengthVector l) {
        m_input_size = l;
        set_needs_relayout();
    }

    sf::Vector2f position() const { return m_pos; }
    sf::Vector2f size() const { return m_size; }
    sf::FloatRect rect() const;
    sf::FloatRect local_rect() const { return { {}, m_size }; }
    LengthVector input_position() const { return m_expected_pos; }
    LengthVector input_size() const { return m_input_size; }

    // FIXME: These should be private somehow.
    virtual void do_relayout();
    virtual void do_draw(GUI::SFMLWindow&) const;

    void set_visible(bool visible) {
        if (m_visible != visible) {
            m_visible = visible;
            set_needs_relayout();
        }
    }

    bool is_visible() const { return m_visible; }

    void set_enabled(bool enabled) { m_enabled = enabled; }
    bool is_enabled() const { return m_enabled; }
    bool are_all_parents_enabled() const;

    void set_focused();
    bool is_focused() const;

    GUI::SFMLWindow& window() const;
    Container* parent() const { return m_parent; }

    void set_tooltip_text(std::string t) { m_tooltip_text = std::move(t); }

    std::string_view id() const { return m_id; }
    std::string_view class_name() const { return m_class_name; }
    void set_id(std::string_view id) { m_id = id; }
    void set_class_name(std::string_view class_name) { m_class_name = class_name; }

    virtual void dump(unsigned depth);

    void set_background_color(sf::Color const& color) { m_background_color = color; }

protected:
    explicit Widget(WidgetTreeRoot& wtr)
        : m_widget_tree_root(wtr) {
    }

    WidgetTreeRoot& widget_tree_root() const { return m_widget_tree_root; }
    Theme const& theme() const;

    virtual void relayout() { }
    virtual bool is_mouse_over(sf::Vector2i) const;
    virtual void update();
    virtual void handle_event(Event&);
    virtual bool accepts_focus() const { return false; }
    virtual void focus_first_child_or_self();

    void set_needs_relayout();

private:
    friend Container;

    virtual LengthVector initial_size() const { return LengthVector {}; }

    Container* m_parent = nullptr;
    WidgetTreeRoot& m_widget_tree_root;
    sf::Vector2f m_pos, m_size;
    LengthVector m_expected_pos, m_input_size;
    TooltipOverlay* m_tooltip = nullptr;
    int m_tooltip_counter = -1;
    std::string m_tooltip_text;
    std::string_view m_id, m_class_name;
    bool m_hover = false;
    bool m_visible = true;
    bool m_enabled = true;
    sf::Color m_background_color = sf::Color::Transparent;
};

}
