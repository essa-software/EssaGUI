#pragma once

#include <EssaGUI/eml/AST.hpp>
#include <EssaGUI/eml/EMLObject.hpp>
#include <EssaGUI/eml/Loader.hpp>
#include <EssaGUI/gfx/ResourceManager.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Theme.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Orientation.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <EssaUtil/Vector.hpp>
#include <cassert>
#include <string>
#include <string_view>

namespace GUI {

#define CREATE_VALUE(Type, camel_case, def_val)        \
protected:                                             \
    Type m_##camel_case = def_val;                     \
                                                       \
public:                                                \
    Type camel_case() const { return m_##camel_case; } \
    void set_##camel_case(Type const& value) { m_##camel_case = value; }

#define CREATE_BOOLEAN(camel_case, def_val)                 \
protected:                                                  \
    bool m_##camel_case = def_val;                          \
                                                            \
public:                                                     \
    bool is_##camel_case() const { return m_##camel_case; } \
    void set_##camel_case(bool const& value) { m_##camel_case = value; }

class Container;
class HostWindow;
class TooltipOverlay;
class WidgetTreeRoot;

struct LengthVector {
    Length x;
    Length y;

    Length main(Util::Orientation o) const {
        if (o == Util::Orientation::Horizontal)
            return x;
        return y;
    }

    Length cross(Util::Orientation o) const {
        if (o == Util::Orientation::Vertical)
            return x;
        return y;
    }
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
    void set_handled() { m_handled = true; }
    bool is_handled() const { return m_handled; }
    void set_seen() { m_seen = true; }
    bool is_seen() const { return m_seen; }

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

    HostWindow& host_window() const;

    virtual void do_handle_event(Event& event);
    virtual void do_update();
    virtual void draw(GUI::Window&) const { }

    CREATE_VALUE(Util::Vector2f, raw_position, Util::Vector2f())
    CREATE_VALUE(Util::Vector2f, raw_size, Util::Vector2f())

    LengthVector position() const { return m_expected_pos; }
    LengthVector size() const { return m_input_size; }

    void set_position(LengthVector l) {
        m_expected_pos = l;
        set_needs_relayout();
    }

    void set_size(LengthVector l) {
        m_input_size = l;
        set_needs_relayout();
    }

    Util::Rectf rect() const;
    Util::Rectf local_rect() const { return { Util::Vector2f(), m_raw_size }; }

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
    bool is_hover() const { return m_hover; }

    CREATE_BOOLEAN(enabled, true);
    CREATE_BOOLEAN(initialized, false);

    bool are_all_parents_enabled() const;

    void set_focused();
    bool is_focused() const;

    Container* parent() const { return m_parent; }

    // FIXME: The names are not the best.
    enum class TooltipMode {
        Hint,     // Shows only if mouse is standing still, after delay, and doesn't move
        Realtime, // Shows always when widget is hovered and follows mouse
    };

    CREATE_VALUE(Util::UString, tooltip_text, "")
    CREATE_VALUE(TooltipMode, tooltip_mode, TooltipMode::Hint)
    CREATE_VALUE(std::string, id, "")
    CREATE_VALUE(std::string, class_name, "")
    CREATE_VALUE(Util::Color, background_color, Util::Colors::Transparent)

    virtual void dump(unsigned depth);
    virtual void default_values() {};

    virtual void eml_construct(WidgetTreeRoot& root) {
        set_widget_tree_root(root);
        if (!m_initialized) {
            on_init();
            m_initialized = true;
        }
    }
    virtual void on_init() { }

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

    // Returns whether tooltip should be displayed at specified widget-relative position.
    virtual bool should_display_tooltip(Util::Vector2f const&) const { return !m_tooltip_text.is_empty(); }

    // Called when tooltip is first shown up. Position is widget-relative.
    virtual Util::UString create_tooltip(Util::Vector2f const&) const { return m_tooltip_text; }

    // Called every tick when tooltip is shown. Position is widget-relative.
    virtual void update_tooltip(Util::Vector2f const&, Util::UString&) const { }

    void set_needs_relayout();

private:
    friend Container;
    friend WidgetTreeRoot;

    void set_widget_tree_root(WidgetTreeRoot& wtr) { m_widget_tree_root = &wtr; }
    void set_parent(Container& parent);

    virtual LengthVector initial_size() const { return LengthVector {}; }

    Container* m_parent = nullptr;
    WidgetTreeRoot* m_widget_tree_root = nullptr;
    LengthVector m_expected_pos;
    LengthVector m_input_size { Length::Initial, Length::Initial };

    TooltipOverlay* m_tooltip = nullptr;
    int m_tooltip_counter = -1;
    Util::Vector2i m_tooltip_position;

    bool m_hover = false;
    bool m_visible = true;
};

}
