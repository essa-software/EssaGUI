#pragma once

#include "Overlay.hpp"
#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Widget.hpp>

namespace GUI {

class ToolWindow : public Overlay {
public:
    explicit ToolWindow(HostWindow&, std::string id = "ToolWindow");

    static constexpr auto TitleBarSize = 28;
    static constexpr auto MinSize = 50;
    static constexpr auto ResizeRadius = 20;

    virtual Util::Vector2f position() const override { return m_position; }
    void set_position(Util::Vector2f position) { m_position = position; }
    void center_on_screen();

    virtual Util::Vector2f size() const override { return m_size; }
    void set_size(Util::Vector2f size) { m_size = size; }

    CREATE_VALUE(Util::UString, title, "")

    virtual Util::Rectf full_rect() const override { return { position() - Util::Vector2f(0, TitleBarSize), size() + Util::Vector2f(0, TitleBarSize) }; }
    Util::Rectf titlebar_rect() const { return { position() - Util::Vector2f(0, TitleBarSize), { size().x(), TitleBarSize } }; }

protected:
    virtual void handle_event(llgl::Event) override;
    virtual void draw(Gfx::Painter&) override;

private:
    virtual void update() override {
        WidgetTreeRoot::update();
        m_first_tick = false;
    }

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    enum class Resize {
        LEFT,
        LEFTBOTTOM,
        BOTTOM,
        RIGHTBOTTOM,
        RIGHT,
        DEFAULT
    };
    Resize m_resize_mode = Resize::DEFAULT;

    Util::Vector2f m_position;
    Util::Vector2f m_size;
    bool m_dragging = false;
    bool m_resizing = false;
    Util::Vector2i m_drag_position;
    bool m_first_tick = true;

    struct TitlebarButton {
        std::function<void()> on_click;
        bool hovered = false;
        bool mousedown = false;
    };
    std::vector<TitlebarButton> m_titlebar_buttons;
};

}
