#pragma once

#include <Essa/GUI/Overlay.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/OpenGL/Framebuffer.hpp>

namespace GUI {

class ToolWindow : public Overlay {
public:
    explicit ToolWindow(HostWindow&, std::string id = "ToolWindow");

    virtual Util::Point2i position() const override { return m_position; }
    void set_position(Util::Point2i position) { m_position = position; }
    void center_on_screen();

    virtual Util::Size2i size() const override { return m_size; }
    void set_size(Util::Size2i size) { m_size = size; }

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
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    Util::Point2i m_position;
    Util::Size2i m_size;
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
