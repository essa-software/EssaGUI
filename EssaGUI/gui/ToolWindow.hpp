#pragma once

#include "Overlay.hpp"

namespace GUI {

class ToolWindow : public Overlay {
public:
    explicit ToolWindow(GUI::Window& wnd, std::string id = "ToolWindow");

    static constexpr auto TitleBarSize = 28;
    static constexpr auto MinSize = 50;
    static constexpr auto ResizeRadius = 20;

    virtual Util::Vector2f position() const override { return m_position; }
    void set_position(Util::Vector2f position) { m_position = position; }
    void center_on_screen() { m_position = Util::Vector2f(window().size().x() / 2, window().size().y() / 2) - m_size / 2.f; }

    virtual Util::Vector2f size() const override { return m_size; }
    void set_size(Util::Vector2f size) { m_size = size; }

    void set_title(Util::UString string) { m_title = std::move(string); }
    Util::UString title() const { return m_title; }

    virtual Util::Rectf full_rect() const override { return { position() - Util::Vector2f(0, TitleBarSize), size() + Util::Vector2f(0, TitleBarSize) }; }
    Util::Rectf titlebar_rect() const { return { position() - Util::Vector2f(0, TitleBarSize), { size().x(), TitleBarSize } }; }

    virtual void handle_event(llgl::Event) override;
    virtual void draw() override;

private:
    virtual void handle_events() override;
    virtual void update() override {
        WidgetTreeRoot::update();
        m_first_tick = false;
    }

    enum class Resize {
        LEFT,
        LEFTBOTTOM,
        BOTTOM,
        RIGHTBOTTOM,
        RIGHT,
        DEFAULT
    };
    Resize m_resize_mode = Resize::DEFAULT;

    Util::UString m_title;
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
