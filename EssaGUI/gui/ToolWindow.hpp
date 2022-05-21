#pragma once

#include "Overlay.hpp"

namespace GUI {

class ToolWindow : public Overlay {
public:
    explicit ToolWindow(sf::RenderWindow& wnd, std::string id = "ToolWindow");

    static constexpr auto TitleBarSize = 28;
    static constexpr auto MinSize = 50;
    static constexpr auto ResizeRadius = 20;

    virtual sf::Vector2f position() const override { return m_position; }
    void set_position(sf::Vector2f position) { m_position = position; }
    void center_on_screen() { m_position = sf::Vector2f(window().getSize() / 2u) - m_size / 2.f; }

    virtual sf::Vector2f size() const override { return m_size; }
    void set_size(sf::Vector2f size) { m_size = size; }

    void set_title(sf::String string) { m_title = std::move(string); }
    sf::String title() const { return m_title; }

    virtual sf::FloatRect full_rect() const override { return { position() - sf::Vector2f(0, TitleBarSize), size() + sf::Vector2f(0, TitleBarSize) }; }
    sf::FloatRect titlebar_rect() const { return { position() - sf::Vector2f(0, TitleBarSize), { size().x, TitleBarSize } }; }

    virtual void handle_event(sf::Event) override;
    virtual void draw() override;

private:
    virtual void handle_events() override;

    enum class Resize {
        LEFT,
        LEFTBOTTOM,
        BOTTOM,
        RIGHTBOTTOM,
        RIGHT,
        DEFAULT
    };
    Resize m_resize_mode = Resize::DEFAULT;

    sf::String m_title;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    bool m_dragging = false;
    bool m_resizing = false;
    sf::Vector2f m_drag_position;

    struct TitlebarButton {
        std::function<void()> on_click;
        bool hovered = false;
        bool mousedown = false;
    };
    std::vector<TitlebarButton> m_titlebar_buttons;
};

}
