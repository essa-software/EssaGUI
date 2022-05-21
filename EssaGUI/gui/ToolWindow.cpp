#include "ToolWindow.hpp"

#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <EssaGUI/gfx/RoundedEdgeRectangleShape.hpp>
#include "Application.hpp"
#include "Widget.hpp"
#include "WidgetTreeRoot.hpp"

#include <SFML/Window/Cursor.hpp>
#include <cassert>
#include <cmath>
#include <iostream>

namespace GUI {

ToolWindow::ToolWindow(sf::RenderWindow& wnd, std::string id)
    : Overlay(wnd, std::move(id)) {
    m_titlebar_buttons.push_back(TitlebarButton {
        .on_click = [this]() {
            close();
        } });
}

void ToolWindow::handle_event(sf::Event event) {
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
    sf::RectangleShape background { size() };
    background.setFillColor(sf::Color(50, 50, 50, 120));
    background.setPosition(position());
    window().draw(background);

    // FIXME: Add some graphical indication that there is
    //        modal window opened now
    auto color = Application::the().focused_overlay() == this ? sf::Color(160, 160, 160, 150) : sf::Color(127, 127, 127, 150);

    RoundedEdgeRectangleShape rs_titlebar({ size().x + 2, TitleBarSize }, 5);
    rs_titlebar.set_border_radius_bottom_left(0);
    rs_titlebar.set_border_radius_bottom_right(0);
    rs_titlebar.setPosition(position() - sf::Vector2f(1, TitleBarSize));
    rs_titlebar.setFillColor(color);
    window().draw(rs_titlebar);

    sf::Text title_text(title(), Application::the().font, 15);
    title_text.setPosition(position() + sf::Vector2f(10, 4 - ToolWindow::TitleBarSize));
    window().draw(title_text);

    float titlebar_button_position_x = position().x + size().x - TitleBarSize + 1;
    for (auto& button : m_titlebar_buttons) {
        // FIXME: For now, there is only a close button. If this becomes not
        //        a case anymore, find a better place for this rendering code.
        //        (And make it more generic)
        RoundedEdgeRectangleShape tbb_background {
            { TitleBarSize, TitleBarSize }, 0
        };
        tbb_background.set_border_radius_top_right(5);
        tbb_background.setFillColor(button.hovered ? sf::Color(240, 80, 80, 100) : sf::Color(200, 50, 50, 100));
        tbb_background.setPosition(titlebar_button_position_x, position().y - TitleBarSize);
        window().draw(tbb_background);

        sf::Vector2f button_center { titlebar_button_position_x + TitleBarSize / 2.f, position().y - TitleBarSize / 2.f };

        sf::VertexArray varr(sf::Lines, 4);
        sf::Color const CloseButtonColor { 200, 200, 200 };
        varr[0] = sf::Vertex(button_center - sf::Vector2f(5, 5), CloseButtonColor);
        varr[1] = sf::Vertex(button_center + sf::Vector2f(5, 5), CloseButtonColor);
        varr[2] = sf::Vertex(button_center - sf::Vector2f(-5, 5), CloseButtonColor);
        varr[3] = sf::Vertex(button_center + sf::Vector2f(-5, 5), CloseButtonColor);
        window().draw(varr);

        titlebar_button_position_x -= TitleBarSize;
    }

    sf::RectangleShape rs_border(size() - sf::Vector2f(0, 1));
    rs_border.setPosition(position() + sf::Vector2f(0, 1));
    rs_border.setFillColor(sf::Color::Transparent);
    rs_border.setOutlineColor(color);
    rs_border.setOutlineThickness(1);
    window().draw(rs_border);
    {
        Gfx::ClipViewScope scope(window(), rect(), Gfx::ClipViewScope::Mode::Override);
        WidgetTreeRoot::draw();
    }
}

}
