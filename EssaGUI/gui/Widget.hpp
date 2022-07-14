#pragma once

#include <EssaGUI/gfx/SFMLWindow.hpp>
#include <EssaGUI/gui/Theme.hpp>
#include <EssaUtil/Units.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
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

    Util::Vector2i mouse_position() const {
        assert(is_mouse_related());
        if (m_event.type == sf::Event::MouseMoved)
            return { m_event.mouseMove.x, m_event.mouseMove.y };
        if (m_event.type == sf::Event::MouseButtonPressed || m_event.type == sf::Event::MouseButtonReleased)
            return { m_event.mouseButton.x, m_event.mouseButton.y };
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
    virtual void draw(GUI::SFMLWindow&) const { }

    void set_raw_position(Util::Vector2f p) {
        m_pos = p;
    }
    void set_raw_size(Util::Vector2f s) {
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

    Util::Vector2f position() const { return m_pos; }
    Util::Vector2f size() const { return m_size; }
    Util::Rectf rect() const;
    Util::Rectf local_rect() const { return { {}, m_size }; }
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

    void set_tooltip_text(Util::UString t) { m_tooltip_text = std::move(t); }

    std::string_view id() const { return m_id; }
    std::string_view class_name() const { return m_class_name; }
    void set_id(std::string_view id) { m_id = id; }
    void set_class_name(std::string_view class_name) { m_class_name = class_name; }

    virtual void dump(unsigned depth);

    void set_background_color(Util::Color const& color) { m_background_color = color; }
    void set_foreground_color(Util::Color const& color) { m_foreground_color = color; }
    void set_text_color(Util::Color const& color) { m_text_color = color; }

    Util::Color get_background_color() const { return m_background_color; }
    Util::Color get_foreground_color() const { return m_foreground_color; }
    Util::Color get_text_color() const { return m_text_color; }

    void set_display_attributes(Util::Color bg_color, Util::Color fg_color, Util::Color text_color) {
        set_background_color(bg_color);
        set_foreground_color(fg_color);
        set_text_color(text_color);
    }

protected:
    explicit Widget(WidgetTreeRoot& wtr)
        : m_widget_tree_root(wtr) {
    }

    WidgetTreeRoot& widget_tree_root() const { return m_widget_tree_root; }
    Theme const& theme() const;

    virtual void relayout() { }
    virtual bool is_mouse_over(Util::Vector2i) const;
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
    Util::Vector2f m_pos, m_size;
    LengthVector m_expected_pos, m_input_size;
    TooltipOverlay* m_tooltip = nullptr;
    int m_tooltip_counter = -1;
    Util::UString m_tooltip_text;
    std::string_view m_id;
    std::string_view m_class_name;
    bool m_hover = false;
    bool m_visible = true;
    bool m_enabled = true;
    Util::Color m_background_color = Util::Colors::transparent;
    Util::Color m_foreground_color = Util::Colors::transparent;
    Util::Color m_text_color = Util::Colors::white;
};

}
