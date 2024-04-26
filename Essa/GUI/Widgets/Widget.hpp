#pragma once

#include <Essa/GUI/DevTools/Object.hpp>
#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/Event.hpp>
#include <Essa/GUI/EventLoop.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Theme.hpp>
#include <Essa/LLGL/Window/Cursor.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Length.hpp>
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
class WindowRoot;

struct LengthVector {
    Util::Length x;
    Util::Length y;

    static LengthVector from_main_cross(Util::Orientation o, Util::Length main, Util::Length cross) {
        return o == Util::Orientation::Horizontal ? LengthVector { main, cross } : LengthVector { cross, main };
    }

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

constexpr bool operator==(LengthVector const& a, LengthVector const& b) { return a.x == b.x && a.y == b.y; }

class Widget
    : public EML::EMLObject
    , public DevToolsObject {
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

    Util::Point2i screen_position() const;
    Util::Point2i host_position() const;
    Util::Point2i absolute_position() const;
    Util::Point2i parent_relative_position() const;

    // Returns bounding box relative to host window's top left corner.
    Util::Recti host_rect() const { return { host_position(), raw_size() }; }
    // Returns bounding box relative to widget tree root's top left corner
    Util::Recti absolute_rect() const { return { absolute_position(), raw_size() }; }
    // Returns bounding box relative to parent. If this is a main widget,
    // this will be equal to absolute_rect().
    Util::Recti parent_relative_rect() const { return { parent_relative_position(), raw_size() }; }
    // Returns bounding box with position = (0, 0)
    Util::Recti local_rect() const { return { {}, m_raw_size }; }

    [[deprecated("Use absolute_position() instead")]] Util::Point2i raw_position() const { return absolute_position(); }
    // This sets widget's position directly. Usually this is done
    // by Layout.
    void set_parent_relative_position(Util::Point2i p);
    [[deprecated("Use set_parent_relative_position() instead")]] void set_raw_position(Util::Point2i);

    CREATE_VALUE(Util::Size2i, raw_size, Util::Size2i())

    LengthVector input_position() const { return m_input_position; }
    LengthVector input_size() const { return m_input_size; }

    // clang-format off
    void set_position(LengthVector l) {
        m_input_position = l;
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
    void set_width(Util::Length l) {
        m_input_size.x = l;
        set_needs_relayout();
    }
    void set_height(Util::Length l) {
        m_input_size.y = l;
        set_needs_relayout();
    }

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

    CREATE_BOOLEAN(enabled, true);
    CREATE_BOOLEAN(initialized, false);

    bool are_all_parents_enabled() const;

    void set_focused();
    virtual bool is_focused() const;

    Widget* parent() const { return m_parent; }

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

    virtual void dump(std::ostream& out, unsigned depth);
    virtual std::vector<DevToolsObject const*> dev_tools_children() const override;
    virtual Util::UString dev_tools_name() const override;

    virtual void eml_construct(WindowRoot& root) {
        set_window_root(root);
        if (!m_initialized) {
            on_init();
            m_initialized = true;
        }
    }
    virtual void on_init() { }

    // "Total size" size of all children of a widget (with padding included), or a widget itself.
    virtual Util::Size2i total_size() const { return raw_size(); }

    void init() {
        if (!m_initialized) {
            on_init();
            m_initialized = true;
        }
    }

    WindowRoot& window_root() const { return *m_window_root; }
    /*restricted(MDI::Overlay) part-of-hack*/ WindowRoot* maybe_window_root() const { return m_window_root; }
    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

    // Use these only if you implement (custom) containers.
    // So e.g GUI::Container, GUI::ScrollableContainer, ...

    /*restricted(Custom Containers)*/ void set_window_root(WindowRoot& wtr) { m_window_root = &wtr; }
    /*restricted(Custom Containers)*/ void set_parent(Widget& parent);

    void set_double_click_enabled(bool dblclk) { m_double_click_enabled = dblclk; }

    virtual llgl::Cursor const& cursor([[maybe_unused]] Util::Point2i local_position) const { return llgl::Cursor::arrow(); }

protected:
    EventHandlerResult handle_event(Event const&);

    virtual EventHandlerResult on_key_press(Event::KeyPress const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_key_release(Event::KeyRelease const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_enter(Event::MouseEnter const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_leave(Event::MouseLeave const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_move(Event::MouseMove const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_double_click(Event::MouseDoubleClick const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_mouse_scroll(Event::MouseScroll const&) { return EventHandlerResult::NotAccepted; }
    virtual EventHandlerResult on_text_input(Event::TextInput const&) { return EventHandlerResult::NotAccepted; }

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual void relayout() { }
    // Get mouse position relative to widget's top left corner.
    Util::Point2i local_mouse_position() const;
    // Check if mouse at given absolute position is over the widget.
    virtual bool is_mouse_over(Util::Point2i) const;
    virtual void update() { }
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
    virtual bool should_display_tooltip(Util::Point2i const&) const { return !m_tooltip_text.is_empty(); }

    // Called when tooltip is first shown up. Position is widget-relative.
    virtual Util::UString create_tooltip(Util::Point2i const&) const { return m_tooltip_text; }

    // Called every tick when tooltip is shown. Position is widget-relative.
    virtual void update_tooltip(Util::Point2i const&, Util::UString&) const { }

    void set_needs_relayout();
    void copy_initial_sizes();

private:
    friend Container;
    friend class WindowRoot;

    virtual LengthVector initial_size() const { return LengthVector {}; }

    Widget* m_parent = nullptr;
    WindowRoot* m_window_root = nullptr;
    // Position, relative to parent container.
    Util::Point2i m_position;
    LengthVector m_input_position;
    LengthVector m_input_size { Util::Length::Initial, Util::Length::Initial };
    Alignment m_horizontal_alignment = Alignment::Start;
    Alignment m_vertical_alignment = Alignment::Start;

    TooltipOverlay* m_tooltip = nullptr;
    // local position
    Util::Point2i m_tooltip_position;
    EventLoop::TimerHandle m_tooltip_timer;

    bool m_double_click_enabled = false;
    EventLoop::TimerHandle m_double_click_timer;

    bool m_hover = false;
    bool m_hovered_on_click = false;
    bool m_visible = true;
};

ESSA_ENUM_FROM_STRING(Widget::Alignment, alignment, ESSAGUI_ENUMERATE_ALIGNMENTS);

}
