#include "ToolWindow.hpp"

#include <EssaGUI/Graphics/ClipViewScope.hpp>
#include <EssaGUI/Graphics/Text.hpp>
#include <EssaGUI/Graphics/Window.hpp>
#include <EssaGUI/GUI/Application.hpp>
#include <EssaGUI/GUI/Widget.hpp>
#include <EssaGUI/GUI/WidgetTreeRoot.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <cassert>
#include <cmath>
#include <iostream>

namespace GUI {

ToolWindow::ToolWindow(HostWindow& window, std::string id)
    : Overlay(window, std::move(id)) {
    m_titlebar_buttons.push_back(TitlebarButton {
        .on_click = [this]() {
            close();
        } });
}

void ToolWindow::handle_event(llgl::Event event) {
    if (m_first_tick)
        return;

    auto& window = host_window().window();

    Event gui_event { event };
    if (gui_event.is_mouse_related()) {
        auto mouse_position = gui_event.mouse_position();
        mouse_position += Util::Vector2i { position() };

        if (event.type == llgl::Event::Type::MouseMove) {
            // TODO
            // bool bottom = mouse_position.y() > position().y() + size().y() - ResizeRadius;
            // bool is_in_window_x_range = mouse_position.x() > position().x() - ResizeRadius && mouse_position.x() < position().x() + size().x() + ResizeRadius;
            // bool is_in_window_y_range = mouse_position.y() > position().y() + TitleBarSize - ResizeRadius && mouse_position.y() < position().y() + size().y() + ResizeRadius;

            // sf::Cursor cursor;
            // if (std::fabs(mouse_position.x() - position().x()) < ResizeRadius && is_in_window_y_range) {
            //     if (bottom && is_in_window_x_range) {
            //         m_resize_mode = Resize::LEFTBOTTOM;
            //         // FIXME: Arch Linux / SFML moment? (doesn't load diagonal size cursors)
            //         cursor.loadFromSystem(sf::Cursor::SizeAll);
            //     }
            //     else {
            //         m_resize_mode = Resize::LEFT;
            //         cursor.loadFromSystem(sf::Cursor::SizeHorizontal);
            //     }
            // }
            // else if (std::fabs(mouse_position.x() - position().x() - size().x()) < ResizeRadius && is_in_window_y_range) {
            //     if (bottom && is_in_window_x_range) {
            //         m_resize_mode = Resize::RIGHTBOTTOM;
            //         // FIXME: Arch Linux / SFML moment? (doesn't load diagonal size cursors)
            //         cursor.loadFromSystem(sf::Cursor::SizeAll);
            //     }
            //     else {
            //         m_resize_mode = Resize::RIGHT;
            //         cursor.loadFromSystem(sf::Cursor::SizeHorizontal);
            //     }
            // }
            // else if (bottom && is_in_window_y_range && is_in_window_x_range) {
            //     m_resize_mode = Resize::BOTTOM;
            //     cursor.loadFromSystem(sf::Cursor::SizeVertical);
            // }
            // else {
            //     m_resize_mode = Resize::DEFAULT;
            //     cursor.loadFromSystem(sf::Cursor::Arrow);
            // }
            // std::cout << "TODO: set mouse cursor" << std::endl;
            // window().setMouseCursor(cursor);
        }

        float titlebar_button_position_x = position().x() + size().x() - TitleBarSize;
        for (auto& button : m_titlebar_buttons) {
            Util::Rectf rect { { titlebar_button_position_x, position().y() - TitleBarSize }, { TitleBarSize, TitleBarSize } };

            if (event.type == llgl::Event::Type::MouseButtonPress) {
                if (rect.contains(mouse_position)) {
                    button.mousedown = true;
                    button.hovered = true;
                }
            }
            else if (event.type == llgl::Event::Type::MouseButtonRelease) {
                button.mousedown = true;
                if (rect.contains(mouse_position)) {
                    assert(button.on_click);
                    button.on_click();
                }
            }
            else if (event.type == llgl::Event::Type::MouseMove) {
                button.hovered = rect.contains(mouse_position);
            }

            titlebar_button_position_x -= TitleBarSize;
        }

        if (event.type == llgl::Event::Type::MouseButtonPress) {
            if (m_resize_mode != Resize::DEFAULT)
                m_resizing = true;
            else if (titlebar_rect().contains(mouse_position)) {
                m_dragging = true;
                m_drag_position = mouse_position;
                return;
            }
        }
        else if (event.type == llgl::Event::Type::MouseMove) {
            Util::Vector2i mouse_position = event.mouse_move.position;
            mouse_position += Util::Vector2i { position() };

            if (m_dragging) {
                auto delta = mouse_position - m_drag_position;
                m_position += Util::Vector2f { delta };
                m_drag_position = mouse_position;

                if (m_position.y() < TitleBarSize)
                    m_position.y() = TitleBarSize;
                if (m_position.y() > window.size().y())
                    m_position.y() = window.size().y();
                if (m_position.x() < -size().x() + TitleBarSize)
                    m_position.x() = -size().x() + TitleBarSize;
                if (m_position.x() > window.size().x() - TitleBarSize)
                    m_position.x() = window.size().x() - TitleBarSize;

                return;
            }
            else if (m_resizing) {
                // sf::Cursor cursor;
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
        else if (event.type == llgl::Event::Type::MouseButtonRelease) {
            m_dragging = false;
            m_resizing = false;
        }
    }
    WidgetTreeRoot::handle_event(event);
}

void ToolWindow::center_on_screen() {
    auto& window = host_window().window();
    m_position = Util::Vector2f(window.size().x() / 2, window.size().y() / 2) - m_size / 2.f;
}

void ToolWindow::draw(Gfx::Painter& painter) {
    Util::Vector2f position { std::round(this->position().x()), std::round(this->position().y()) };
    Util::Vector2f size { std::round(this->size().x()), std::round(this->size().y()) };

    Gfx::RectangleDrawOptions background;
    background.fill_color = { 50, 50, 50, 220 };
    painter.draw_rectangle({ position, size }, background);

    // FIXME: Add some graphical indication that there is
    //        modal window opened now
    auto titlebar_color = host_window().focused_overlay() == this ? Util::Color { 120, 120, 120, 220 } : Util::Color { 80, 80, 80, 220 };

    Gfx::RectangleDrawOptions rs_titlebar;
    rs_titlebar.border_radius_top_left = 5;
    rs_titlebar.border_radius_top_right = 5;
    rs_titlebar.fill_color = titlebar_color;
    painter.draw_rectangle({ position - Util::Vector2f(1, TitleBarSize), { size.x() + 2, TitleBarSize } }, rs_titlebar);

    Gfx::Text text { title(), Application::the().bold_font() };
    text.set_position({ position + Util::Vector2f(10, -TitleBarSize / 2.f + 5) });
    text.set_font_size(theme().label_font_size);
    text.set_fill_color(Util::Colors::White);
    text.draw(painter);

    float titlebar_button_position_x = position.x() + size.x() - TitleBarSize + 1;
    for (auto& button : m_titlebar_buttons) {
        // FIXME: For now, there is only a close button. If this becomes not
        //        a case anymore, find a better place for this rendering code.
        //        (And make it more generic)
        Gfx::RectangleDrawOptions tbb_background;
        tbb_background.border_radius_top_right = 5;
        tbb_background.fill_color = button.hovered ? Util::Color { 240, 80, 80, 100 } : Util::Color { 200, 50, 50, 100 };
        painter.draw_rectangle({ { titlebar_button_position_x, position.y() - TitleBarSize }, { TitleBarSize, TitleBarSize } }, tbb_background);

        Util::Vector2f button_center { std::round(titlebar_button_position_x + TitleBarSize / 2.f) - 1, std::round(position.y() - TitleBarSize / 2.f) - 1 };

        std::array<Gfx::Vertex, 4> varr;
        Util::Color const CloseButtonColor { 200, 200, 200 };
        varr[0] = Gfx::Vertex { button_center - Util::Vector2f(5, 5), CloseButtonColor, {} };
        varr[1] = Gfx::Vertex { button_center + Util::Vector2f(5, 5), CloseButtonColor, {} };
        varr[2] = Gfx::Vertex { button_center - Util::Vector2f(-5, 5), CloseButtonColor, {} };
        varr[3] = Gfx::Vertex { button_center + Util::Vector2f(-5, 5), CloseButtonColor, {} };
        painter.draw_vertices(llgl::PrimitiveType::Lines, varr);

        titlebar_button_position_x -= TitleBarSize;
    }

    std::array<Gfx::Vertex, 4> varr_border;
    varr_border[0] = Gfx::Vertex { { position }, titlebar_color, {} };
    varr_border[1] = Gfx::Vertex { { position + Util::Vector2f(0, size.y() + 1) }, titlebar_color, {} };
    varr_border[2] = Gfx::Vertex { { position + Util::Vector2f(size.x() + 1, size.y() + 1) }, titlebar_color, {} };
    varr_border[3] = Gfx::Vertex { { position + Util::Vector2f(size.x() + 1, 0) }, titlebar_color, {} };
    painter.draw_vertices(llgl::PrimitiveType::LineStrip, varr_border);
    {
        Gfx::ClipViewScope scope(painter, Util::Vector2u { host_window().size() }, rect(), Gfx::ClipViewScope::Mode::Override);
        WidgetTreeRoot::draw(painter);
    }
}

EML::EMLErrorOr<void> ToolWindow::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Overlay::load_from_eml_object(object, loader));

    m_title = TRY(object.get_property("title", Util::UString {}).to_string());
    m_size.x() = TRY(object.get_property("width", 0.0).to_double());
    m_size.y() = TRY(object.get_property("height", 0.0).to_double());
    if (TRY(object.get_property("center_on_screen", false).to_bool())) {
        center_on_screen();
    }

    return {};
}

}
