#include "Grid.hpp"

#include <EssaGUI/gui/Container.hpp>
#include <EssaUtil/Units.hpp>
#include <stdexcept>

namespace GUI {

Border* Grid::get_cell(size_t x, size_t y) {
    if (m_width <= x || m_height <= y) {
        throw std::runtime_error("Grid indexes out of bounds!");
    }

    return m_cells[y][x];
}

void Grid::set_height(size_t height) {

    m_cells.resize(height);
    shrink(height);

    for (size_t i = m_height; i < height; i++) {
        auto line = add_widget<Container>();
        line->set_layout<HorizontalBoxLayout>().set_spacing(m_spacing);
        line->set_size({ Length::Auto, { m_line_height, Length::Px } });

        m_cells[i].resize(m_width);

        for (size_t j = 0; j < m_width; j++) {
            m_cells[i][j] = line->add_widget<Border>();
        }
    }

    m_height = height;
}

void Grid::set_width(size_t width) {
    size_t iter = 0;

    for (auto& line : m_widgets) {
        auto con = dynamic_cast<Container*>(line.get());

        if (m_width <= width) {
            con->shrink(width);
            m_cells[iter].resize(width);
        }
        else {
            for (size_t i = m_width; i < width; i++) {
                m_cells[iter].push_back(con->add_widget<Border>());
            }
        }

        iter++;
    }

    m_width = width;
}

void Grid::resize(size_t width, size_t height) {
    m_width = width;
    set_height(height);
}

void Grid::set_spacing(float spacing) {
    dynamic_cast<BoxLayout*>(get_layout().get())->set_spacing(spacing);

    for (auto& line : m_widgets) {
        auto con = dynamic_cast<Container*>(line.get());

        dynamic_cast<BoxLayout*>(con->get_layout().get())->set_spacing(spacing);
    }

    m_spacing = spacing;
}

void Grid::set_line_height(float height) {
    m_line_height = height;

    for (auto& line : m_widgets) {
        line->set_size({ Length::Auto, { m_line_height, Length::Px } });
    }
}

}
