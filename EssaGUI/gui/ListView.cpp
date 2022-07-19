#include "ListView.hpp"

#include "Application.hpp"
#include "EssaGUI/gui/ContextMenu.hpp"
#include "EssaGUI/gui/ScrollableWidget.hpp"
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <LLGL/Window/Mouse.hpp>
#include <variant>

namespace GUI {

constexpr float RowHeight = 30;

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

void ListView::draw(GUI::Window& wnd) const {
    assert(m_model);

    size_t rows = m_model->row_count();
    size_t columns = m_model->column_count();

    size_t first_row = -scroll_offset().y() / RowHeight;
    if (first_row > 0)
        first_row--;
    size_t last_row = std::min<size_t>(rows, (-scroll_offset().y() + scroll_area_height()) / RowHeight);
    if (last_row < rows - 1)
        last_row++;

    float current_x_pos = 0;
    // TODO: Limit display to widget size
    // TODO: Scrolling
    // TODO: Many more things...

    // Background
    if (rows > 0) {
        for (size_t r = first_row; r < last_row; r++) {
            Util::Color bg_color = r % 2 == 0 ? Util::Color { 100, 100, 100, 128 } : Util::Color { 80, 80, 80, 128 };
            RectangleDrawOptions rs;
            rs.fill_color = bg_color;
            wnd.draw_rectangle({ Util::Vector2f { 0, RowHeight * (r + 1) } + scroll_offset(), { size().x(), RowHeight } }, rs);
        }
    }

    // Column names
    {
        RectangleDrawOptions rs;
        rs.fill_color = { 200, 200, 200, 128 };
        wnd.draw_rectangle({ scroll_offset(), { size().x(), RowHeight } }, rs);

        float x_pos = 0;
        for (size_t c = 0; c < columns; c++) {
            auto column = m_model->column(c);
            TextDrawOptions text;
            text.font_size = 16;
            wnd.draw_text(column.name, Application::the().bold_font, Util::Vector2f { x_pos + 5, 20 } + scroll_offset(), text);
            x_pos += column.width;
        }
    }

    // Data
    if (rows > 0) {
        for (size_t c = 0; c < columns; c++) {
            auto column = m_model->column(c);

            for (size_t r = first_row; r < last_row; r++) {
                auto data = m_model->data(r, c);
                Util::Vector2f cell_position { current_x_pos, (r + 1) * RowHeight };
                cell_position += scroll_offset();
                auto cell_size = this->cell_size(r, c);

                // TODO: ClipViewScope it

                // TODO: Make this all (font, font size, alignment) configurable

                std::visit(
                    overloaded {
                        [&](Util::UString const& data) {
                            TextDrawOptions text;
                            text.font_size = 15;
                            text.text_align = Align::CenterLeft;
                            wnd.draw_text_aligned_in_rect(data, { cell_position + Util::Vector2f(5, 0), cell_size }, Application::the().bold_font, text);
                        },
                        [&](llgl::opengl::Texture const* data) {
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
    return Util::Vector2f { position().x(), position().y() + RowHeight * (row + 1) } + scroll_offset();
}

Util::Vector2f ListView::cell_size(size_t, size_t column) const {
    return { m_model->column(column).width, RowHeight };
}

void ListView::handle_event(Event& event) {
    ScrollableWidget::handle_event(event);

    size_t rows = m_model->row_count();
    if (event.type() == llgl::Event::Type::MouseButtonPress) {
        auto mouse_pos = event.mouse_position();

        if (is_mouse_over(mouse_pos)) {
            for (size_t row = 0; row < rows; row++) {
                Util::Vector2f cell_position = row_position(row);
                Util::Rectf rect(cell_position, { size().x(), RowHeight });

                if (rect.contains(mouse_pos)) {
                    if (event.event().mouse_button.button == llgl::MouseButton::Left && on_click) {
                        on_click(row);
                    }
                    else if (event.event().mouse_button.button == llgl::MouseButton::Right && on_context_menu_request) {
                        if (auto context_menu = on_context_menu_request(row)) {
                            GUI::Application::the().open_context_menu(*context_menu, Util::Vector2f { mouse_pos } + widget_tree_root().position());
                        }
                    }
                    return;
                }
            }
        }
    }
}

float ListView::content_height() const {
    return (m_model->row_count() + 1) * RowHeight;
}

}
