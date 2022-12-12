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
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

void ListView::draw(Gfx::Painter& wnd) const {
    auto row_height = theme().line_height;
    auto row_width = this->row_width();

    auto& model = this->model();

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
            wnd.draw_rectangle({ Util::Vector2f { 0, row_height * (display_header() ? r + 1 : r) } + scroll_offset(), { row_width, row_height } }, rs);
        }
    }

    // Column names
    if (display_header()) {
        Gfx::RectangleDrawOptions rs;
        rs.fill_color = theme().text_button.normal.unhovered.background;
        wnd.draw_rectangle({ scroll_offset(), { row_width, row_height } }, rs);

        float x_pos = 0;
        for (size_t c = 0; c < columns; c++) {
            auto column = model.column(c);
            Gfx::Text text { column.name, Application::the().bold_font() };
            text.set_font_size(16);
            text.set_position(Util::Vector2f { x_pos + 5, 20 } + scroll_offset());
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
                Util::Vector2f cell_position { current_x_pos, r * row_height };
                if (display_header())
                    cell_position.y() += row_height;
                cell_position += scroll_offset();
                auto cell_size = this->cell_size(r, c);

                // TODO: ClipViewScope it

                // TODO: Make this all (font, font raw_size, alignment) configurable

                std::visit(
                    overloaded {
                        [&](Util::UString const& data) {
                            Gfx::Text text { data, Application::the().bold_font() };
                            text.set_font_size(theme().label_font_size);
                            text.set_fill_color(c % 2 == 0 ? list_even.text : list_odd.text);
                            text.align(Align::CenterLeft, { cell_position + Util::Vector2f(5, 0), cell_size });
                            text.draw(wnd);
                        },
                        [&](llgl::Texture const* data) {
                            Gfx::RectangleDrawOptions rect;
                            rect.texture = data;
                            wnd.draw_rectangle({ { cell_position.x() + cell_size.x() / 2 - 8, cell_position.y() + cell_size.y() / 2 - 8 }, { 16, 16 } }, rect);
                        } },
                    data);
            }
            // FIXME: Double lookup, probably harmless but still
            current_x_pos += column.width;
        }
    }

    ScrollableWidget::draw_scrollbar(wnd);
}

Widget::EventHandlerResult ListView::on_mouse_button_press(Event::MouseButtonPress const& event) {
    ScrollableWidget::on_mouse_button_press(event);

    size_t rows = model().root_row_count();
    auto mouse_pos = event.local_position();

    if (is_mouse_over(mouse_pos)) {
        for (size_t row = 0; row < rows; row++) {
            Util::Vector2f cell_position = row_position(row);
            Util::Rectf rect(cell_position, { raw_size().x(), theme().line_height });

            if (rect.contains(mouse_pos)) {
                if (event.button() == llgl::MouseButton::Left && on_click) {
                    on_click(row);
                }
                else if (event.button() == llgl::MouseButton::Right && on_context_menu_request) {
                    if (auto context_menu = on_context_menu_request(row)) {
                        host_window().open_context_menu(*context_menu, Util::Vector2f { mouse_pos } + widget_tree_root().position());
                    }
                }
                return EventHandlerResult::NotAccepted;
            }
        }
    }
    return EventHandlerResult::NotAccepted;
}

}
