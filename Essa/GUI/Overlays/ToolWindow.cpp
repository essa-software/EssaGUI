#include "ToolWindow.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/OpenGL/Framebuffer.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <EssaUtil/Config.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

namespace GUI {

ToolWindow::ToolWindow(HostWindow& window, std::string id)
    : Overlay(window, std::move(id))
    , m_window_shadow(GUI::Application::the().resource_manager().require_texture("misc/window_shadow.png")) {
    m_titlebar_buttons.push_back(TitlebarButton { .on_click = [this]() { close(); } });
}

void ToolWindow::handle_event(Event const& event) {
    if (m_first_tick)
        return;

    auto& window = host_window();

    bool should_pass_event_to_widgets = true;
    if (event.is_mouse_related()) {
        auto mouse_position = event.local_mouse_position();
        mouse_position += position().to_vector();

        float titlebar_button_position_x = position().x() + size().x() - theme().tool_window_title_bar_size;
        for (auto& button : m_titlebar_buttons) {
            Util::Rectf rect { { titlebar_button_position_x, position().y() - theme().tool_window_title_bar_size },
                               { theme().tool_window_title_bar_size, theme().tool_window_title_bar_size } };

            if (event.is<Event::MouseButtonPress>()) {
                if (rect.contains(mouse_position)) {
                    button.mousedown = true;
                    button.hovered = true;
                }
            }
            else if (event.is<Event::MouseButtonRelease>()) {
                button.mousedown = true;
                if (rect.contains(mouse_position)) {
                    assert(button.on_click);
                    button.on_click();
                }
            }
            else if (event.is<Event::MouseMove>()) {
                button.hovered = rect.contains(mouse_position);
            }

            titlebar_button_position_x -= theme().tool_window_title_bar_size;
        }

        if (event.is<Event::MouseButtonPress>()) {
            auto start_dragging = [&]() {
                m_drag_position = { mouse_position.x(), mouse_position.y() };
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
                    if (i >= m_resize_directions.size()) {
                        break;
                    }
                    m_resize_directions[i++] = direction;
                    start_dragging();
                }
            }
        }
        else if (event.is<Event::MouseMove>()) {
            auto is_resizing = std::any_of(m_resize_directions.begin(), m_resize_directions.end(), [](std::optional<ResizeDirection> r) {
                return r.has_value();
            });

            auto delta = mouse_position - m_drag_position.to_vector();

            auto constrain_position = [this, &window]() {
                if (m_position.y() < static_cast<int>(theme().tool_window_title_bar_size))
                    m_position.set_y(theme().tool_window_title_bar_size);
                if (m_position.y() > static_cast<int>(window.size().y()))
                    m_position.set_y(window.size().y());
                if (m_position.x() < static_cast<int>(-size().x() + theme().tool_window_title_bar_size))
                    m_position.set_x(-size().x() + theme().tool_window_title_bar_size);
                if (m_position.x() > static_cast<int>(window.size().x() - theme().tool_window_title_bar_size))
                    m_position.set_x(window.size().x() - theme().tool_window_title_bar_size);
            };

            if (m_moving) {
                m_position = m_initial_dragging_position + delta.to_vector();
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
                        if (m_initial_dragging_size.x() - delta.x() < static_cast<int>(theme().tool_window_min_size)) {
                            delta.set_x(m_initial_dragging_size.x() - theme().tool_window_min_size);
                        }
                        if (m_initial_dragging_position.x() + delta.x() < 0) {
                            delta.set_x(-m_initial_dragging_position.x());
                        }
                        m_size.set_x(m_initial_dragging_size.x() - delta.x());
                        m_position.set_x(m_initial_dragging_position.x() + delta.x());
                        break;
                    case ResizeDirection::Top:
                        if (m_initial_dragging_size.y() - delta.y() < static_cast<int>(theme().tool_window_min_size)) {
                            delta.set_y(m_initial_dragging_size.y() - theme().tool_window_min_size);
                        }
                        if (m_initial_dragging_position.y() + delta.y() < static_cast<int>(theme().tool_window_title_bar_size)) {
                            delta.set_y(-m_initial_dragging_position.y() + theme().tool_window_title_bar_size);
                        }
                        m_size.set_y(m_initial_dragging_size.y() - delta.y());
                        m_position.set_y(m_initial_dragging_position.y() + delta.y());
                        break;
                    case ResizeDirection::Right:
                        if (m_initial_dragging_size.x() + delta.x() < static_cast<int>(theme().tool_window_min_size)) {
                            delta.set_x(theme().tool_window_min_size - m_initial_dragging_size.x());
                        }
                        m_size.set_x(m_initial_dragging_size.x() + delta.x());
                        break;
                    case ResizeDirection::Bottom:
                        if (m_initial_dragging_size.y() + delta.y() < static_cast<int>(theme().tool_window_min_size)) {
                            delta.set_y(theme().tool_window_min_size - m_initial_dragging_size.y());
                        }
                        m_size.set_y(m_initial_dragging_size.y() + delta.y());
                        break;
                    }
                }

                // Send resize event to ToolWindow's widgets
                WidgetTreeRoot::handle_event(llgl::Event::WindowResize {
                    { static_cast<unsigned>(m_size.x()), static_cast<unsigned>(m_size.y()) } });
                return;
            }
        }
        else if (event.is<Event::MouseButtonRelease>()) {
            m_moving = false;
            m_resize_directions = {};
        }
    }

    if (should_pass_event_to_widgets) {
        WidgetTreeRoot::handle_event(event);
    }
}

void ToolWindow::center_on_screen() {
    auto& window = host_window();
    m_position = (window.size() / 2 - m_size / 2.f).to_vector().to_point();
}

void ToolWindow::draw(Gfx::Painter& painter) {
    using namespace Gfx::Drawing;

    if (is_modal()) {
        Gfx::RectangleDrawOptions modal_backdrop;
        modal_backdrop.fill_color = theme().modal_backdrop;
        painter.deprecated_draw_rectangle(host_window().rect().cast<float>(), modal_backdrop);
    }

    Util::Cs::Point2f position { std::round(this->position().x()), std::round(this->position().y()) };
    Util::Cs::Size2f size { std::round(this->size().x()), std::round(this->size().y()) };

    {
        // FIXME: Assuming that shadow is equal in x and y.
        auto shadow_radius = (static_cast<float>(m_window_shadow.size().x()) - 1) / 2;
        auto shadow_rect = full_rect()
                               .inflated(shadow_radius - static_cast<float>(theme().tool_window_resize_border_width) / 2)
                               .inflated_horizontal(1)
                               .cast<float>();

        auto texture_rect = Util::Rectf({}, m_window_shadow.size().cast<float>());

        Util::Color const shadow_color = Util::Colors::White.with_alpha(255 * theme().tool_window_shadow_opacity);

        // Sides
        painter.draw(Rectangle(
            shadow_rect.take_left(shadow_radius).inflated_vertical(-shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_left(shadow_radius).inflated_vertical(-shadow_radius)) //
                .set_color(shadow_color)
        ));
        painter.draw(Rectangle(
            shadow_rect.take_right(shadow_radius).inflated_vertical(-shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_right(shadow_radius).inflated_vertical(-shadow_radius))
                .set_color(shadow_color)
        ));
        painter.draw(Rectangle(
            shadow_rect.take_top(shadow_radius).inflated_horizontal(-shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_top(shadow_radius).inflated_horizontal(-shadow_radius))
                .set_color(shadow_color)
        ));
        painter.draw(Rectangle(
            shadow_rect.take_bottom(shadow_radius).inflated_horizontal(-shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_bottom(shadow_radius).inflated_horizontal(-shadow_radius))
                .set_color(shadow_color)
        ));

        // Corners
        painter.draw(Rectangle(
            shadow_rect.take_left(shadow_radius).take_top(shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_left(shadow_radius).take_top(shadow_radius)) //
                .set_color(shadow_color)
        ));
        painter.draw(Rectangle(
            shadow_rect.take_left(shadow_radius).take_bottom(shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_left(shadow_radius).take_bottom(shadow_radius)) //
                .set_color(shadow_color)
        ));
        painter.draw(Rectangle(
            shadow_rect.take_right(shadow_radius).take_top(shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_right(shadow_radius).take_top(shadow_radius)) //
                .set_color(shadow_color)
        ));
        painter.draw(Rectangle(
            shadow_rect.take_right(shadow_radius).take_bottom(shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.take_right(shadow_radius).take_bottom(shadow_radius)) //
                .set_color(shadow_color)
        ));

        // Fill
        painter.draw(Rectangle(
            shadow_rect.inflated(-shadow_radius),
            Fill::textured(m_window_shadow, texture_rect.inflated(-shadow_radius)) //
                .set_color(shadow_color)
        ));
    }

    auto titlebar_color
        = host_window().focused_overlay() == this ? theme().tab_button.active.unhovered : theme().tab_button.inactive.unhovered;

    Gfx::RectangleDrawOptions rs_titlebar;
    rs_titlebar.border_radius_top_left = theme().tool_window_title_bar_border_radius;
    rs_titlebar.border_radius_top_right = theme().tool_window_title_bar_border_radius;
    rs_titlebar.fill_color = titlebar_color.background;
    painter.deprecated_draw_rectangle(
        { position - Util::Cs::Vector2f(1, theme().tool_window_title_bar_size), { size.x() + 2, theme().tool_window_title_bar_size } },
        rs_titlebar
    );

    Gfx::Text text { title(), Application::the().bold_font() };
    text.set_position((position + Util::Cs::Vector2f(10, -(theme().tool_window_title_bar_size / 2.f) + 5)).to_deprecated_vector());
    text.set_font_size(theme().label_font_size);
    text.set_fill_color(titlebar_color.text);
    text.draw(painter);

    float titlebar_button_position_x = position.x() + size.x() - theme().tool_window_title_bar_size + 1;
    for (auto& button : m_titlebar_buttons) {
        // FIXME: For now, there is only a close button. If this becomes not
        //        a case anymore, find a better place for this rendering code.
        //        (And make it more generic)
        Gfx::RectangleDrawOptions tbb_background;
        tbb_background.border_radius_top_right = theme().tool_window_title_bar_border_radius;
        tbb_background.fill_color = theme().negative;
        if (button.hovered) {
            tbb_background.fill_color = tbb_background.fill_color * theme().hover_highlight_factor;
        }
        painter.deprecated_draw_rectangle(
            { { titlebar_button_position_x, position.y() - theme().tool_window_title_bar_size },
              { theme().tool_window_title_bar_size, theme().tool_window_title_bar_size } },
            tbb_background
        );

        Util::Vector2f button_center { std::round(titlebar_button_position_x + theme().tool_window_title_bar_size / 2.f),
                                       std::round(position.y() - theme().tool_window_title_bar_size / 2.f) };

        std::array<Gfx::Vertex, 4> varr;
        auto close_button_cross_color = theme().text_button.active.unhovered.text;
        varr[0] = Gfx::Vertex { button_center - Util::Vector2f(5, 5), close_button_cross_color, {} };
        varr[1] = Gfx::Vertex { button_center + Util::Vector2f(5, 5), close_button_cross_color, {} };
        varr[2] = Gfx::Vertex { button_center - Util::Vector2f(-5, 5), close_button_cross_color, {} };
        varr[3] = Gfx::Vertex { button_center + Util::Vector2f(-5, 5), close_button_cross_color, {} };
        painter.draw_vertices(llgl::PrimitiveType::Lines, varr);

        titlebar_button_position_x -= theme().tool_window_title_bar_size;
    }

    std::array<Gfx::Vertex, 4> varr_border;
    varr_border[0] = Gfx::Vertex { { position.to_deprecated_vector() }, titlebar_color.background, {} };
    varr_border[1] = Gfx::Vertex { { position.to_deprecated_vector() + Util::Vector2f(-1, size.y()) }, titlebar_color.background, {} };
    varr_border[2]
        = Gfx::Vertex { { position.to_deprecated_vector() + Util::Vector2f(size.x() + 1, size.y()) }, titlebar_color.background, {} };
    varr_border[3] = Gfx::Vertex { { position.to_deprecated_vector() + Util::Vector2f(size.x() + 1, 0) }, titlebar_color.background, {} };
    painter.draw_vertices(llgl::PrimitiveType::LineStrip, varr_border);

    // Flush because of text being draw with incorrect blending mode otherwise
    painter.render();
    painter.reset();

    m_backing_buffer.resize(size.cast<unsigned>());
    m_offscreen_painter.reset();
    m_offscreen_painter.renderer().clear(Util::Colors::Transparent);
    {
        Gfx::ClipViewScope scope(m_offscreen_painter, Util::Recti { {}, size.cast<int>() }, Gfx::ClipViewScope::Mode::NewStack);

        m_offscreen_painter.draw(
            Rectangle(Util::Rectf({}, size), Fill::solid(theme().window_background.with_alpha(255 * theme().tool_window_opacity)))
        );

        WidgetTreeRoot::draw(m_offscreen_painter);
    }
    m_offscreen_painter.render();

    auto const& texture = m_backing_buffer.color_texture();
    painter.draw(Rectangle(
        Util::Rectf(position.cast<float>(), size.cast<float>()),
        Fill::textured(texture, { 0, 0, static_cast<float>(texture.size().x()), -static_cast<float>(texture.size().y()) })
            .set_color(Util::Colors::White.with_alpha(255 * theme().tool_window_opacity))
    ));
    auto blending = painter.blending();
    // Disable alpha premultiplying because it was already done
    painter.set_blending(Gfx::Painter::Blending {
        .src_rgb = Gfx::Painter::BlendingFunc::One,
        .dst_rgb = Gfx::Painter::BlendingFunc::OneMinusSrcAlpha,
        .src_alpha = Gfx::Painter::BlendingFunc::One,
        .dst_alpha = Gfx::Painter::BlendingFunc::OneMinusSrcAlpha,
    });
    // Flush because of blending
    painter.render();
    painter.reset();
    painter.set_blending(blending);
}

Util::Recti ToolWindow::resize_rect(ResizeDirection direction) const {
    switch (direction) {
    case ResizeDirection::Top:
        return full_rect().take_top(theme().tool_window_resize_border_width);
    case ResizeDirection::Right:
        return full_rect().take_right(theme().tool_window_resize_border_width);
    case ResizeDirection::Bottom:
        return full_rect().take_bottom(theme().tool_window_resize_border_width);
    case ResizeDirection::Left:
        return full_rect().take_left(theme().tool_window_resize_border_width);
    }
    ESSA_UNREACHABLE;
}

EML::EMLErrorOr<void> ToolWindow::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Overlay::load_from_eml_object(object, loader));

    m_title = TRY(object.get_property("title", EML::Value(Util::UString {})).to_string());
    m_size.set_x(TRY(object.get_property("width", EML::Value(0.0)).to_double()));
    m_size.set_y(TRY(object.get_property("height", EML::Value(0.0)).to_double()));
    if (TRY(object.get_property("center_on_screen", EML::Value(false)).to_bool())) {
        center_on_screen();
    }

    return {};
}
}
