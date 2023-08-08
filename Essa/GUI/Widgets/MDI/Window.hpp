#pragma once

#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/OpenGL/Framebuffer.hpp>
#include <Essa/LLGL/Window/WindowSettings.hpp>

namespace GUI::MDI {

class Host;

class Window final : public WidgetTreeRoot {
public:
    explicit Window(Host& host);

    virtual void setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const&) override;

    virtual void close() override { m_closed = true; }
    bool is_closed() const { return m_closed; }

    virtual void center_on_screen() override;

    virtual Util::Point2i position() const override { return m_position; }
    void set_position(Util::Point2i position) { m_position = position; }
    virtual Util::Size2i size() const override { return m_size; }
    virtual void set_size(Util::Size2i size) override { m_size = size; }
    virtual HostWindow& host_window() override;

    Host& host() { return m_host; }
    Host const& host() const { return m_host; }

    std::function<void()> on_close;

    // If true, window will not receive any events except Global events, or if explicitly focused.
    void set_ignores_events(bool pass) { m_ignore_events = pass; }
    bool ignores_events() const { return m_ignore_events; }

    // If true, focusing a window won't bring it to the top.
    void set_always_on_bottom(bool value) { m_always_on_bottom = value; }
    bool always_on_bottom() const { return m_always_on_bottom; }

    CREATE_VALUE(Util::UString, title, "")

    virtual Util::Recti full_rect() const override {
        return Util::Recti {
            position() - Util::Vector2i(0, theme().tool_window_title_bar_size),
            size() + Util::Vector2i(0, theme().tool_window_title_bar_size),
        }
            .inflated(static_cast<int>(theme().tool_window_resize_border_width / 2));
    }
    Util::Recti titlebar_rect() const {
        return {
            position() - Util::Vector2i(0, theme().tool_window_title_bar_size),
            { size().x(), theme().tool_window_title_bar_size },
        };
    }

    enum class ResizeDirection { Top, Right, Bottom, Left };

    Util::Recti resize_rect(ResizeDirection direction) const;

protected:
    virtual void handle_event(GUI::Event const&) override;
    virtual void draw(Gfx::Painter&) override;

    virtual void update() override {
        WidgetTreeRoot::update();
        m_first_tick = false;
    }

private:
    friend class Host;

    // Deprecated. Override WindowRoot::load_from_eml_object for window-agnostic EML loaders.
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    void draw_decorations(Gfx::Painter&) const;

    Util::Point2i m_position;
    Util::Size2i m_size;
    Host& m_host;
    bool m_closed = false;
    bool m_ignore_events = false;
    bool m_always_on_bottom = false;
    llgl::WindowSettings m_settings;

    bool m_moving = false;
    std::array<std::optional<ResizeDirection>, 2> m_resize_directions;
    Util::Point2i m_initial_dragging_position;
    Util::Size2i m_initial_dragging_size;
    // FIXME: Use DragBehavior
    Util::Point2i m_drag_position;
    bool m_first_tick = true;

    struct TitlebarButton {
        std::function<void()> on_click;
        bool hovered = false;
        bool mousedown = false;
    };
    std::vector<TitlebarButton> m_titlebar_buttons;
    llgl::Framebuffer m_backing_buffer { size().cast<unsigned>() };
    Gfx::Painter m_offscreen_painter { m_backing_buffer.renderer() };
    llgl::Texture const& m_window_shadow;
};

}
