#pragma once

#include <EssaGUI/eml/EMLObject.hpp>
#include <EssaGUI/gfx/ResourceManager.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Theme.hpp>
#include <EssaUtil/Units.hpp>
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
    Event(llgl::Event const& event)
        : m_event(event) {
    }

    llgl::Event event() const { return m_event; }

    // FIXME: idk the names
    bool is_handled() const { return m_handled; }
    void set_handled() { m_handled = true; }
    bool is_seen() const { return m_seen; }
    void set_seen() { m_seen = true; }

    llgl::Event::Type type() const { return m_event.type; }

    bool is_mouse_related() const {
        return m_event.type == llgl::Event::Type::MouseMove || m_event.type == llgl::Event::Type::MouseButtonPress || m_event.type == llgl::Event::Type::MouseButtonRelease;
    }

    Util::Vector2i mouse_position() const {
        assert(is_mouse_related());
        if (m_event.type == llgl::Event::Type::MouseMove)
            return m_event.mouse_move.position;
        if (m_event.type == llgl::Event::Type::MouseButtonPress || m_event.type == llgl::Event::Type::MouseButtonRelease)
            return m_event.mouse_button.position;
        __builtin_unreachable();
    }

private:
    llgl::Event m_event;
    bool m_handled = false;
    bool m_seen = false;
};

class Widget : public EML::EMLObject {
public:
    Widget() = default;
    Widget(Widget const&) = delete;
    Widget& operator=(Widget const&) = delete;

    virtual ~Widget();

    bool is_hover() const { return m_hover; }

    virtual void do_handle_event(Event& event);
    virtual void do_update();
    virtual void draw(GUI::Window&) const { }

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
    virtual void do_draw(GUI::Window&) const;

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

    GUI::Window& window() const;
    Container* parent() const { return m_parent; }

    void set_tooltip_text(Util::UString t) { m_tooltip_text = std::move(t); }

    std::string_view id() const { return m_id; }
    std::string_view class_name() const { return m_class_name; }
    void set_id(std::string_view id) { m_id = id; }
    void set_class_name(std::string_view class_name) { m_class_name = class_name; }

    virtual void dump(unsigned depth);

    void set_background_color(Util::Color const& color) { m_background_color = color; }

    virtual void eml_construct(WidgetTreeRoot& root) { set_widget_tree_root(root); }

protected:
    WidgetTreeRoot& widget_tree_root() const { return *m_widget_tree_root; }
    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual void relayout() { }
    virtual bool is_mouse_over(Util::Vector2i) const;
    virtual void update();
    virtual void handle_event(Event&);
    virtual bool accepts_focus() const { return false; }

    // "Steals focus" - so that the widget cannot be focused from outside
    // and the focus cannot "escape" the widget using Tab. Used for settings
    // windows and settings menu so that you can "circulate" all settings
    // using Tab. Also used for multiline TextEditor so that you can use
    // Tab to indent text.
    // FIXME: Allow user to set it for any widget.
    virtual bool steals_focus() const { return false; }

    virtual void focus_first_child_or_self();

    void set_needs_relayout();

private:
    friend Container;
    friend WidgetTreeRoot;

    void set_widget_tree_root(WidgetTreeRoot& wtr) { m_widget_tree_root = &wtr; }
    void set_parent(Container& parent);

    virtual LengthVector initial_size() const { return LengthVector {}; }

    Container* m_parent = nullptr;
    WidgetTreeRoot* m_widget_tree_root = nullptr;
    Util::Vector2f m_pos, m_size;
    LengthVector m_expected_pos;
    LengthVector m_input_size { Length::Initial, Length::Initial };
    TooltipOverlay* m_tooltip = nullptr;
    int m_tooltip_counter = -1;
    Util::UString m_tooltip_text;
    std::string m_id;
    std::string_view m_class_name;
    bool m_hover = false;
    bool m_visible = true;
    bool m_enabled = true;
    Util::Color m_background_color = Util::Colors::Transparent;
};

}
