#include "ListView.hpp"

#include "Application.hpp"
#include "EssaGUI/gui/ContextMenu.hpp"
#include "EssaGUI/gui/ScrollableWidget.hpp"
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <LLGL/Window/Mouse.hpp>
#include <variant>

namespace GUI {

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

void ListView::draw(GUI::Window& wnd) const {
    auto row_height = theme().line_height;

    assert(m_model);

    size_t rows = m_model->row_count();
    size_t columns = m_model->column_count();

    size_t first_row = -scroll_offset().y() / row_height;
    if (first_row > 0)
        first_row--;
    size_t last_row = std::min<size_t>(rows, (-scroll_offset().y() + scroll_area_height()) / row_height);
    if (last_row < rows - 1)
        last_row++;

    float current_x_pos = 0;
    // TODO: Limit display to widget raw_size
    // TODO: Scrolling
    // TODO: Many more things...

    auto list_odd = theme().list_odd;
    auto list_even = theme().list_even;

    // Background
    if (rows > 0) {
        for (size_t r = first_row; r < last_row; r++) {
            RectangleDrawOptions rs;
            rs.fill_color = r % 2 == 0 ? list_even.background : list_odd.background;
            wnd.draw_rectangle({ Util::Vector2f { 0, row_height * (r + 1) } + scroll_offset(), { raw_size().x(), row_height } }, rs);
        }
    }

    // Column names
    {
        RectangleDrawOptions rs;
        rs.fill_color = theme().text_button.normal.unhovered.background;
        wnd.draw_rectangle({ scroll_offset(), { raw_size().x(), row_height } }, rs);

        float x_pos = 0;
        for (size_t c = 0; c < columns; c++) {
            auto column = m_model->column(c);
            TextDrawOptions text;
            text.font_size = 16;
            wnd.draw_text(column.name, Application::the().bold_font(), Util::Vector2f { x_pos + 5, 20 } + scroll_offset(), text);
            x_pos += column.width;
        }
    }

    // Data
    if (rows > 0) {
        for (size_t c = 0; c < columns; c++) {
            auto column = m_model->column(c);

            for (size_t r = first_row; r < last_row; r++) {
                auto data = m_model->data(r, c);
                Util::Vector2f cell_position { current_x_pos, (r + 1) * row_height };
                cell_position += scroll_offset();
                auto cell_size = this->cell_size(r, c);

                // TODO: ClipViewScope it

                // TODO: Make this all (font, font raw_size, alignment) configurable

                std::visit(
                    overloaded {
                        [&](Util::UString const& data) {
                            TextDrawOptions text;
                            text.font_size = theme().label_font_size;
                            text.text_align = Align::CenterLeft;
                            text.fill_color = c % 2 == 0 ? list_even.text : list_odd.text;
                            wnd.draw_text_aligned_in_rect(data, { cell_position + Util::Vector2f(5, 0), cell_size }, Application::the().bold_font(), text);
                        },
                        [&](llgl::Texture const* data) {
                            RectangleDrawOptions rect;
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

Util::Vector2f ListView::row_position(unsigned row) const {
    return Util::Vector2f { raw_position().x(), raw_position().y() + theme().line_height * (row + 1) } + scroll_offset();
}

Util::Vector2f ListView::cell_size(size_t, size_t column) const {
    return { m_model->column(column).width, theme().line_height };
}

void ListView::handle_event(Event& event) {
    ScrollableWidget::handle_event(event);

    size_t rows = m_model->row_count();
    if (event.type() == llgl::Event::Type::MouseButtonPress) {
        auto mouse_pos = event.mouse_position();

        if (is_mouse_over(mouse_pos)) {
            for (size_t row = 0; row < rows; row++) {
                Util::Vector2f cell_position = row_position(row);
                Util::Rectf rect(cell_position, { raw_size().x(), theme().line_height });

                if (rect.contains(mouse_pos)) {
                    if (event.event().mouse_button.button == llgl::MouseButton::Left && on_click) {
                        on_click(row);
                    }
                    else if (event.event().mouse_button.button == llgl::MouseButton::Right && on_context_menu_request) {
                        if (auto context_menu = on_context_menu_request(row)) {
                            host_window().open_context_menu(*context_menu, Util::Vector2f { mouse_pos } + widget_tree_root().position());
                        }
                    }
                    return;
                }
            }
        }
    }
}

float ListView::content_height() const {
    return (m_model->row_count() + 1) * theme().line_height;
}

}
