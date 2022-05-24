#include "ToolWindow.hpp"

#include "Application.hpp"
#include "EssaGUI/gfx/SFMLWindow.hpp"
#include "Widget.hpp"
#include "WidgetTreeRoot.hpp"

#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <SFML/Window/Cursor.hpp>
#include <cassert>
#include <cmath>
#include <iostream>

namespace GUI {

ToolWindow::ToolWindow(GUI::SFMLWindow& wnd, std::string id)
    : Overlay(wnd, std::move(id)) {
    m_titlebar_buttons.push_back(TitlebarButton {
        .on_click = [this]() {
            close();
        } });
}

void ToolWindow::handle_event(sf::Event event) {
    if (m_first_tick)
        return;

    Event gui_event { event };
    if (gui_event.is_mouse_related()) {
        sf::Vector2f mouse_position = gui_event.mouse_position();
        mouse_position += position();

        if (event.type == sf::Event::MouseMoved) {
            bool bottom = mouse_position.y > position().y + size().y - ResizeRadius;
            bool is_in_window_x_range = mouse_position.x > position().x - ResizeRadius && mouse_position.x < position().x + size().x + ResizeRadius;
            bool is_in_window_y_range = mouse_position.y > position().y + TitleBarSize - ResizeRadius && mouse_position.y < position().y + size().y + ResizeRadius;

            sf::Cursor cursor;
            if (std::fabs(mouse_position.x - position().x) < ResizeRadius && is_in_window_y_range) {
                if (bottom && is_in_window_x_range) {
                    m_resize_mode = Resize::LEFTBOTTOM;
                    // FIXME: Arch Linux / SFML moment? (doesn't load diagonal size cursors)
                    cursor.loadFromSystem(sf::Cursor::SizeAll);
                }
                else {
                    m_resize_mode = Resize::LEFT;
                    cursor.loadFromSystem(sf::Cursor::SizeHorizontal);
                }
            }
            else if (std::fabs(mouse_position.x - position().x - size().x) < ResizeRadius && is_in_window_y_range) {
                if (bottom && is_in_window_x_range) {
                    m_resize_mode = Resize::RIGHTBOTTOM;
                    // FIXME: Arch Linux / SFML moment? (doesn't load diagonal size cursors)
                    cursor.loadFromSystem(sf::Cursor::SizeAll);
                }
                else {
                    m_resize_mode = Resize::RIGHT;
                    cursor.loadFromSystem(sf::Cursor::SizeHorizontal);
                }
            }
            else if (bottom && is_in_window_y_range && is_in_window_x_range) {
                m_resize_mode = Resize::BOTTOM;
                cursor.loadFromSystem(sf::Cursor::SizeVertical);
            }
            else {
                m_resize_mode = Resize::DEFAULT;
                cursor.loadFromSystem(sf::Cursor::Arrow);
            }
            window().setMouseCursor(cursor);
        }

        float titlebar_button_position_x = position().x + size().x - TitleBarSize;
        for (auto& button : m_titlebar_buttons) {
            sf::FloatRect rect { { titlebar_button_position_x, position().y - TitleBarSize }, { TitleBarSize, TitleBarSize } };

            if (event.type == sf::Event::MouseButtonPressed) {
                if (rect.contains(mouse_position)) {
                    button.mousedown = true;
                    button.hovered = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                button.mousedown = true;
                if (rect.contains(mouse_position)) {
                    assert(button.on_click);
                    button.on_click();
                }
            }
            else if (event.type == sf::Event::MouseMoved) {
                button.hovered = rect.contains(mouse_position);
            }

            titlebar_button_position_x -= TitleBarSize;
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (m_resize_mode != Resize::DEFAULT)
                m_resizing = true;
            else if (titlebar_rect().contains(mouse_position)) {
                m_dragging = true;
                m_drag_position = mouse_position;
                return;
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mouse_position { static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y) };
            mouse_position += position();

            if (m_dragging) {
                auto delta = mouse_position - m_drag_position;
                m_position += delta;
                m_drag_position = mouse_position;

                if (m_position.y < TitleBarSize)
                    m_position.y = TitleBarSize;
                if (m_position.y > window().getSize().y)
                    m_position.y = window().getSize().y;
                if (m_position.x < -size().x + TitleBarSize)
                    m_position.x = -size().x + TitleBarSize;
                if (m_position.x > window().getSize().x - TitleBarSize)
                    m_position.x = window().getSize().x - TitleBarSize;

                return;
            }
            else if (m_resizing) {
                sf::Cursor cursor;
                switch (m_resize_mode) {
                case Resize::LEFT:

                    break;
                case Resize::LEFTBOTTOM:

                    break;
                case Resize::BOTTOM:

                    break;
                case Resize::RIGHTBOTTOM:

                    break;
                case Resize::RIGHT:

                    break;
                case Resize::DEFAULT:

                    break;
                }
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            m_dragging = false;
            m_resizing = false;
        }
    }
    WidgetTreeRoot::handle_event(event);
}

void ToolWindow::handle_events() {
    // This event handler just takes all the events
    // (except global events) and passes the to the
    // underlying  main_widget. This is used for modal
    // windows.
    // FIXME: Support moving other ToolWindows even
    //        if other modal window is open.
    sf::Event event;
    while (window().pollEvent(event)) {
        handle_event(transform_event(position(), event));
        if (event.type == sf::Event::Resized || event.type == sf::Event::Closed)
            Application::the().handle_event(event);
    }
}

void ToolWindow::draw() {
    sf::Vector2f position { std::round(this->position().x), std::round(this->position().y) };
    sf::Vector2f size { std::round(this->size().x), std::round(this->size().y) };

    RectangleDrawOptions background;
    background.fill_color = sf::Color(50, 50, 50, 180);
    window().draw_rectangle({ position, size }, background);

    // FIXME: Add some graphical indication that there is
    //        modal window opened now
    auto color = Application::the().focused_overlay() == this ? sf::Color(160, 160, 160, 150) : sf::Color(127, 127, 127, 150);

    RectangleDrawOptions rs_titlebar;
    rs_titlebar.border_radius_top_left = 5;
    rs_titlebar.border_radius_top_right = 5;
    rs_titlebar.fill_color = color;
    window().draw_rectangle({ position - sf::Vector2f(1, TitleBarSize), { size.x + 2, TitleBarSize } }, rs_titlebar);

    window().draw_text(title(), Application::the().bold_font, { position + sf::Vector2f(10, -TitleBarSize / 2.f + 5) }, { .font_size = 15 });

    float titlebar_button_position_x = position.x + size.x - TitleBarSize + 1;
    for (auto& button : m_titlebar_buttons) {
        // FIXME: For now, there is only a close button. If this becomes not
        //        a case anymore, find a better place for this rendering code.
        //        (And make it more generic)
        RectangleDrawOptions tbb_background;
        tbb_background.border_radius_top_right = 5;
        tbb_background.fill_color = button.hovered ? sf::Color(240, 80, 80, 100) : sf::Color(200, 50, 50, 100);
        window().draw_rectangle({ { titlebar_button_position_x, position.y - TitleBarSize }, { TitleBarSize, TitleBarSize } }, tbb_background);

        sf::Vector2f button_center { std::round(titlebar_button_position_x + TitleBarSize / 2.f) - 1, std::round(position.y - TitleBarSize / 2.f) - 1 };

        std::array<Vertex, 4> varr;
        sf::Color const CloseButtonColor { 200, 200, 200 };
        varr[0] = Vertex { .position = Vector3 { button_center - sf::Vector2f(5, 5) }, .color = CloseButtonColor };
        varr[1] = Vertex { .position = Vector3 { button_center + sf::Vector2f(5, 5) }, .color = CloseButtonColor };
        varr[2] = Vertex { .position = Vector3 { button_center - sf::Vector2f(-5, 5) }, .color = CloseButtonColor };
        varr[3] = Vertex { .position = Vector3 { button_center + sf::Vector2f(-5, 5) }, .color = CloseButtonColor };
        window().draw_vertices(GL_LINES, varr);

        titlebar_button_position_x -= TitleBarSize;
    }

    std::array<Vertex, 4> varr_border;
    varr_border[0] = Vertex { .position = Vector3 { position }, .color = color };
    varr_border[1] = Vertex { .position = Vector3 { position + sf::Vector2f(0, size.y + 1) }, .color = color };
    varr_border[2] = Vertex { .position = Vector3 { position + sf::Vector2f(size.x + 1, size.y + 1) }, .color = color };
    varr_border[3] = Vertex { .position = Vector3 { position + sf::Vector2f(size.x + 1, 0) }, .color = color };
    window().draw_vertices(GL_LINE_STRIP, varr_border);
    {
        Gfx::ClipViewScope scope(window(), rect(), Gfx::ClipViewScope::Mode::Override);
        WidgetTreeRoot::draw();
    }
}

}
