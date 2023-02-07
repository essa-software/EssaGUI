#pragma once

#include <Essa/GUI/EML/AST.hpp>
#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Theme.hpp>
#include <Essa/LLGL/Window/Event.hpp>
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
    Util::Length x;
    Util::Length y;

    Util::Length main(Util::Orientation o) const {
        if (o == Util::Orientation::Horizontal)
            return x;
        return y;
    }

    Util::Length cross(Util::Orientation o) const {
        if (o == Util::Orientation::Vertical)
            return x;
        return y;
    }
};

constexpr bool operator==(LengthVector const& a, LengthVector const& b) {
    return a.x == b.x && a.y == b.y;
}

struct Event : public llgl::Event {
public:
    template<class T>
    Event(T&& t)
        : llgl::Event(std::forward<T>(t)) { }
};

class Widget : public EML::EMLObject {
public:
    Widget() = default;
    Widget(Widget const&) = delete;
    Widget& operator=(Widget const&) = delete;

    virtual ~Widget();

    HostWindow& host_window() const;

    bool is_affected_by_event(Event const&) const;

    enum class EventHandlerResult {
        Accepted,   // Event should not bubble up to Container
        NotAccepted // Event will bubble up
    };

    virtual Widget::EventHandlerResult do_handle_event(Event const& event);
    virtual void do_update();
    virtual void draw(Gfx::Painter&) const { }

    CREATE_VALUE(Util::Vector2f, raw_position, Util::Vector2f())
    CREATE_VALUE(Util::Vector2f, raw_size, Util::Vector2f())

    Util::Vector2f position_on_host_window() const;
    Util::Vector2f position_on_widget_tree_root() const;

    LengthVector position() const { return m_expected_pos; }
    LengthVector size() const { return m_input_size; }

    // clang-format off
    void set_position(LengthVector l) {
        m_expected_pos = l;
        set_needs_relayout();
    }

    // Alignment is applicably only for BasicLayout.
    #define ESSAGUI_ENUMERATE_ALIGNMENTS(Ex)    \
        Ex(Start)                               \
        Ex(Center)                              \
        Ex(End)
    // clang-format on

    ESSA_ENUM(Alignment, ESSAGUI_ENUMERATE_ALIGNMENTS)

    // Set horizontal alignment of widget in the container (aka align-self)
    void set_horizontal_alignment(Alignment a) {
        m_horizontal_alignment = a;
        set_needs_relayout();
    }
    // Set vertical alignment of widget in the container (aka align-self)
    void set_vertical_alignment(Alignment a) {
        m_vertical_alignment = a;
        set_needs_relayout();
    }
    // Set alignment of widget in the container (aka align-self)
    void set_alignments(Alignment h, Alignment v) {
        m_horizontal_alignment = h;
        m_vertical_alignment = v;
        set_needs_relayout();
    }
    Alignment horizontal_alignment() const { return m_horizontal_alignment; }
    Alignment vertical_alignment() const { return m_vertical_alignment; }

    void set_size(LengthVector l) {
        m_input_size = l;
        set_needs_relayout();
    }

    Util::Rectf rect() const;
    Util::Rectf local_rect() const { return { {}, Util::Cs::Size2f::from_deprecated_vector(m_raw_size) }; }

    // FIXME: These should be private somehow.
    virtual void do_relayout();
    virtual void do_draw(Gfx::Painter&) const;

    void set_visible(bool visible) {
        if (m_visible != visible) {
            m_visible = visible;
            set_needs_relayout();
        }
    }

    bool is_visible() const { return m_visible; }
    bool is_hover() const { return m_hover; }
    bool was_hovered_on_mouse_press() const { return m_hovered_on_click; }

    CREATE_BOOLEAN(enabled, true);
    CREATE_BOOLEAN(initialized, false);

    bool are_all_parents_enabled() const;

    void set_focused();
    virtual bool is_focused() const;

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

    // "Total size" size of all children of a widget (with padding included), or a widget itself.
    virtual Util::Vector2f total_size() const { return raw_size(); }

    void init() {
        if (!m_initialized) {
            on_init();
            m_initialized = true;
        }
    }

    // FIXME: This should probably be private.
    void set_widget_tree_root(WidgetTreeRoot& wtr) { m_widget_tree_root = &wtr; }

protected:
    WidgetTreeRoot& widget_tree_root() const { return *m_widget_tree_root; }
    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

    EventHandlerResult handle_event(Event const&);

    virtual EventHandlerResult on_window_resize(Event::WindowResize const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_key_press(Event::KeyPress const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_key_release(Event::KeyRelease const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_move(Event::MouseMove const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_scroll(Event::MouseScroll const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_text_input(Event::TextInput const&) { return EventHandlerResult::NotAccepted; }

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual void relayout() { }
    virtual bool is_mouse_over(Util::Vector2i) const;
    virtual void update();
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

    void set_parent(Container& parent);

    virtual LengthVector initial_size() const { return LengthVector {}; }

    Container* m_parent = nullptr;
    WidgetTreeRoot* m_widget_tree_root = nullptr;
    LengthVector m_expected_pos;
    LengthVector m_input_size { Util::Length::Initial, Util::Length::Initial };
    Alignment m_horizontal_alignment = Alignment::Start;
    Alignment m_vertical_alignment = Alignment::Start;

    TooltipOverlay* m_tooltip = nullptr;
    int m_tooltip_counter = -1;
    Util::Vector2i m_tooltip_position;

    bool m_hover = false;
    bool m_hovered_on_click = false;
    bool m_visible = true;
};

ESSA_ENUM_FROM_STRING(Widget::Alignment, alignment, ESSAGUI_ENUMERATE_ALIGNMENTS);

}
