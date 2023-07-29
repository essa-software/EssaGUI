#include "ListView.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Widgets/ScrollableWidget.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>
#include <variant>

namespace GUI {

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

void ListView::draw(Gfx::Painter& wnd) const {
    theme().renderer().draw_text_editor_border(*this, false, wnd);

    if (!model()) {
        return;
    }

    auto row_height = theme().line_height;
    auto row_width = this->row_width();

    auto& model = *this->model();

    size_t rows = model.row_count();
    size_t columns = model.column_count();

    size_t first_row = -scroll_offset().y() / row_height;
    if (first_row > 0)
        first_row--;
    size_t last_row = std::min<size_t>(rows, (-scroll_offset().y() + scroll_area_size().y()) / row_height + 1);
    if (last_row < rows - 1)
        last_row++;

    float current_x_pos = 0;

    auto list_odd = theme().list_odd;
    auto list_even = theme().list_even;

    // Background
    if (rows > 0) {
        for (size_t r = first_row; r < last_row; r++) {
            Gfx::RectangleDrawOptions rs;
            rs.fill_color = r % 2 == 0 ? list_even.background : list_odd.background;
            wnd.deprecated_draw_rectangle(
                {
                    Util::Point2f { 0, row_height * (display_header() ? r + 1 : r) } + scroll_offset().cast<float>(),
                    { row_width, row_height },
                },
                rs
            );
        }
    }

    // Column names
    if (display_header()) {
        Gfx::RectangleDrawOptions rs;
        rs.fill_color = theme().text_button.normal.unhovered.background;
        wnd.deprecated_draw_rectangle({ scroll_offset().cast<float>().to_point(), { row_width, row_height } }, rs);

        float x_pos = 0;
        for (size_t c = 0; c < columns; c++) {
            auto column = model.column(c);
            Gfx::Text text { column.name, Application::the().bold_font() };
            text.set_font_size(16);
            text.set_position(Util::Point2f { x_pos + 5, 20 } + scroll_offset().cast<float>());
            text.draw(wnd);
            x_pos += column.width;
        }
    }

    // Data
    if (rows > 0) {
        for (size_t c = 0; c < columns; c++) {
            auto column = model.column(c);

            for (size_t r = first_row; r < last_row; r++) {
                auto data = model.root_data(r, c);
                Util::Point2i cell_position { current_x_pos, r * row_height };
                if (display_header())
                    cell_position.set_y(cell_position.y() + row_height);
                cell_position += scroll_offset();
                auto cell_size = this->cell_size(r, c);

                // TODO: ClipViewScope it

                // TODO: Make this all (font, font raw_size, alignment)
                // configurable

                std::visit(
                    Util::Overloaded {
                        [&](Util::UString const& data) {
                            Gfx::Text text { data, Application::the().bold_font() };
                            text.set_font_size(theme().label_font_size);
                            text.set_fill_color(c % 2 == 0 ? list_even.text : list_odd.text);
                            text.align(
                                Align::CenterLeft, { (cell_position + Util::Vector2i(5, 0)).cast<float>(), cell_size.cast<float>() }
                            );
                            text.draw(wnd);
                        },
                        [&](Gfx::RichText const& data) {
                            Gfx::RichTextDrawable drawable { data,
                                                             {
                                                                 .default_font = Application::the().font(),
                                                                 .font_size = static_cast<int>(theme().label_font_size),
                                                                 .text_alignment = GUI::Align::CenterLeft,
                                                             } };
                            drawable.set_rect({ (cell_position + Util::Vector2i(5, 0)).cast<float>(), cell_size.cast<float>() });
                            drawable.draw(wnd);
                        },
                        [&](llgl::Texture const* data) {
                            Gfx::RectangleDrawOptions rect;
                            rect.texture = data;
                            wnd.deprecated_draw_rectangle(
                                { { cell_position.x() + cell_size.x() / 2 - 8, cell_position.y() + cell_size.y() / 2 - 8 }, { 16, 16 } },
                                rect
                            );
                        } },
                    data
                );
            }
            // FIXME: Double lookup, probably harmless but still
            current_x_pos += column.width;
        }
    }

    ScrollableWidget::draw_scrollbar(wnd);
}

Widget::EventHandlerResult ListView::on_mouse_button_press(Event::MouseButtonPress const& event) {
    ScrollableWidget::on_mouse_button_press(event);

    if (!model()) {
        return Widget::EventHandlerResult::NotAccepted;
    }

    size_t rows = model()->root_row_count();
    auto mouse_pos = event.local_position();

    for (size_t row = 0; row < rows; row++) {
        auto cell_position = row_position(row);
        Util::Recti rect(cell_position, { raw_size().x(), theme().line_height });

        if (rect.contains(mouse_pos)) {
            if (event.button() == llgl::MouseButton::Left && on_click) {
                on_click(row);
            }
            else if (event.button() == llgl::MouseButton::Right && on_context_menu_request) {
                if (auto context_menu = on_context_menu_request(row)) {
                    host_window().open_context_menu(*context_menu, mouse_pos + window_root().window().position().to_vector());
                }
            }
            return EventHandlerResult::NotAccepted;
        }
    }
    return EventHandlerResult::NotAccepted;
}

EML_REGISTER_CLASS(ListView);
}
