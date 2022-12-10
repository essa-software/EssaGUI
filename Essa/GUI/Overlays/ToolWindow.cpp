#include "ToolWindow.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Graphics/Window.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <EssaUtil/Config.hpp>
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
    bool should_pass_event_to_widgets = true;
    if (gui_event.is_mouse_related()) {
        auto mouse_position = gui_event.mouse_position();
        mouse_position += Util::Vector2i { position() };

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
            auto start_dragging = [&]() {
                m_drag_position = mouse_position;
                should_pass_event_to_widgets = false;
                m_initial_dragging_position = m_position;
                m_initial_dragging_size = m_size;
            };

            if (titlebar_rect().contains(mouse_position)) {
                m_moving = true;
                start_dragging();
            }

            size_t i = 0;
            for (auto direction : { ResizeDirection::Top, ResizeDirection::Right, ResizeDirection::Bottom, ResizeDirection::Left }) {
                auto resize_rect = this->resize_rect(direction);
                if (resize_rect.contains(mouse_position)) {
                    assert(i < m_resize_directions.size());
                    m_resize_directions[i++] = direction;
                    start_dragging();
                }
            }
        }
        else if (event.type == llgl::Event::Type::MouseMove) {
            Util::Vector2i mouse_position = event.mouse_move.position;
            mouse_position += Util::Vector2i { position() };

            auto is_resizing = std::any_of(m_resize_directions.begin(), m_resize_directions.end(), [](std::optional<ResizeDirection> r) {
                return r.has_value();
            });

            auto delta = mouse_position - m_drag_position;

            auto constrain_position = [this, &window]() {
                if (m_position.y() < TitleBarSize)
                    m_position.y() = TitleBarSize;
                if (m_position.y() > window.size().y())
                    m_position.y() = window.size().y();
                if (m_position.x() < -size().x() + TitleBarSize)
                    m_position.x() = -size().x() + TitleBarSize;
                if (m_position.x() > window.size().x() - TitleBarSize)
                    m_position.x() = window.size().x() - TitleBarSize;
            };

            if (m_moving) {
                m_position = m_initial_dragging_position + Util::Vector2f { delta };
                constrain_position();
                return;
            }
            if (is_resizing) {
                for (auto direction : m_resize_directions) {
                    if (!direction) {
                        break;
                    }
                    switch (*direction) {
                    case ResizeDirection::Left:
                        if (m_initial_dragging_size.x() - delta.x() < MinSize) {
                            delta.x() = m_initial_dragging_size.x() - MinSize;
                        }
                        if (m_initial_dragging_position.x() + delta.x() < 0) {
                            delta.x() = -m_initial_dragging_position.x();
                        }
                        m_size.x() = m_initial_dragging_size.x() - delta.x();
                        m_position.x() = m_initial_dragging_position.x() + delta.x();
                        break;
                    case ResizeDirection::Top:
                        if (m_initial_dragging_size.y() - delta.y() < MinSize) {
                            delta.y() = m_initial_dragging_size.y() - MinSize;
                        }
                        if (m_initial_dragging_position.y() + delta.y() < TitleBarSize) {
                            delta.y() = -m_initial_dragging_position.y() + TitleBarSize;
                        }
                        m_size.y() = m_initial_dragging_size.y() - delta.y();
                        m_position.y() = m_initial_dragging_position.y() + delta.y();
                        break;
                    case ResizeDirection::Right:
                        if (m_initial_dragging_size.x() + delta.x() < MinSize) {
                            delta.x() = MinSize - m_initial_dragging_size.x();
                        }
                        m_size.x() = m_initial_dragging_size.x() + delta.x();
                        break;
                    case ResizeDirection::Bottom:
                        if (m_initial_dragging_size.y() + delta.y() < MinSize) {
                            delta.y() = MinSize - m_initial_dragging_size.y();
                        }
                        m_size.y() = m_initial_dragging_size.y() + delta.y();
                        break;
                    }
                }

                // constrain_position();

                llgl::Event event;
                event.type = llgl::Event::Type::Resize;
                event.resize.size = { static_cast<int>(m_size.x()), static_cast<int>(m_size.y()) };
                WidgetTreeRoot::handle_event(event);
                return;
            }
        }
        else if (event.type == llgl::Event::Type::MouseButtonRelease) {
            m_moving = false;
            m_resize_directions = {};
        }
    }

    if (should_pass_event_to_widgets) {
        WidgetTreeRoot::handle_event(event);
    }
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

Util::Rectf ToolWindow::resize_rect(ResizeDirection direction) const {
    switch (direction) {
    case ResizeDirection::Top:
        return full_rect().take_top(ResizeBorderWidth);
    case ResizeDirection::Right:
        return full_rect().take_right(ResizeBorderWidth);
    case ResizeDirection::Bottom:
        return full_rect().take_bottom(ResizeBorderWidth);
    case ResizeDirection::Left:
        return full_rect().take_left(ResizeBorderWidth);
    }
    ESSA_UNREACHABLE;
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
