#include "ListView.hpp"

#include <EssaGUI/gfx/ClipViewScope.hpp>
#include "Application.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

namespace GUI {

float const RowHeight = 30;
float const Padding = 5;

void ListView::draw(sf::RenderWindow& wnd) const {
    assert(m_model);

    size_t rows = m_model->row_count();
    size_t columns = m_model->column_count();

    float current_x_pos = 0;
    // TODO: Limit display to widget size
    // TODO: Scrolling
    // TODO: Many more things...

    // Background
    for (size_t r = 0; r < rows; r++) {
        sf::Color bg_color = r % 2 == 0 ? sf::Color { 100, 100, 100, 128 } : sf::Color { 80, 80, 80, 128 };
        sf::RectangleShape rs({ size().x, RowHeight });
        rs.setPosition(0, RowHeight * (r + 1));
        rs.setFillColor(bg_color);
        wnd.draw(rs);
    }

    // Column names
    {
        sf::RectangleShape rs({ size().x, RowHeight });
        rs.setFillColor({ 200, 200, 200, 128 });
        wnd.draw(rs);

        float x_pos = 0;
        for (size_t c = 0; c < columns; c++) {
            auto column = m_model->column(c);
            sf::Text text(column.name, Application::the().bold_font, 16);
            text.setPosition({x_pos + 5, 5});
            wnd.draw(text);
            x_pos += column.width;
        }
    }

    // Data
    for (size_t c = 0; c < columns; c++) {
        auto column = m_model->column(c);

        for (size_t r = 0; r < rows; r++) {
            auto data = m_model->data(r, c);
            sf::Vector2f cell_position { current_x_pos + Padding, (r + 1) * RowHeight + Padding };

            // TODO: ClipViewScope it

            // TODO: Make this all (font, font size, alignment) configurable
            sf::Text text(data, Application::the().font, 15);
            text.setPosition(cell_position);
            wnd.draw(text);
        }
        // FIXME: Double lookup, probably harmless but still
        current_x_pos += column.width;
    }
}

sf::Vector2f ListView::cell_size(size_t, size_t column) const {
    return { m_model->column(column).width, RowHeight };
}

void ListView::do_handle_event(Event &event){
    size_t rows = m_model->row_count();
    if(event.type() == sf::Event::MouseButtonPressed){
        sf::Vector2i mouse_pos(event.mouse_position());

        if(is_mouse_over(mouse_pos)){
            for (size_t r = 0; r < rows; r++) {
                sf::Rect<int> rect(position().x, position().y + RowHeight * (r + 1), size().x, RowHeight);

                if(rect.contains(mouse_pos)){
                    if(on_click)
                        on_click(r);
                    return;
                }
            }
        }
    }
}

}
