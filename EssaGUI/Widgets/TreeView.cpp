#include "TreeView.hpp"

#include <EssaGUI/Application.hpp>
#include "EssaGUI/Graphics/Painter.hpp"
#include <EssaGUI/Graphics/Text.hpp>
#include <EssaGUI/Widgets/AbstractListView.hpp>
#include <EssaGUI/Widgets/ScrollableWidget.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/Window/Mouse.hpp>

namespace GUI {

constexpr float IndentSize = 24;

void TreeView::handle_event(GUI::Event& event) {
    AbstractListView::handle_event(event);
    if (event.type() == llgl::Event::Type::MouseButtonPress && event.event().mouse_button.button == llgl::MouseButton::Left) {
        if (!is_mouse_over(event.mouse_position())) {
            return;
        }
        size_t row = (event.mouse_position().y() - raw_position().y() - scroll_offset().y()) / theme().line_height;
        auto path = displayed_row_at_index(row);
        if (!path.first.empty()) {
            if (m_expanded_paths.contains(path.first)) {
                m_expanded_paths.erase(path.first);
            }
            else {
                expand(path.first);
            }
        }
    }
}

void TreeView::draw(Gfx::Painter& wnd) const {
    auto& model = this->model();

    auto row_height = theme().line_height;
    auto row_width = this->row_width();

    size_t rows = displayed_row_count();
    size_t columns = model.column_count();

    size_t first_row = -scroll_offset().y() / row_height;
    if (first_row > 0)
        first_row--;
    size_t last_row = std::min<size_t>(rows, (-scroll_offset().y() + scroll_area_size().y()) / row_height + 1);
    if (last_row < rows - 1)
        last_row++;

    auto list_odd = theme().list_odd;
    auto list_even = theme().list_even;

    // fmt::print("rows = {}/ render: {}..{}\n", rows, first_row, last_row);

    // Background
    if (rows > 0) {
        for (size_t r = first_row; r < last_row; r++) {
            Gfx::RectangleDrawOptions rs;
            rs.fill_color = r % 2 == 0 ? list_even.background : list_odd.background;
            wnd.draw_rectangle({ Util::Vector2f { 0, row_height * (display_header() ? r + 1 : r) } + scroll_offset(), { row_width, row_height } }, rs);
        }
    }

    // Column names
    float current_y_pos = 0;
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

            x_pos += cell_size(0, c).x();
        }
        current_y_pos += row_height;
    }

    render_rows(wnd, current_y_pos, {}, nullptr);

    ScrollableWidget::draw_scrollbar(wnd);
}

void TreeView::render_rows(Gfx::Painter& painter, float& current_y_pos, std::vector<size_t> path, Model::Node const* parent) const {
    auto& model = this->model();
    auto columns = model.column_count();
    auto depth = path.size();

    auto list_odd = theme().list_odd;
    auto list_even = theme().list_even;
    auto row_height = theme().line_height;

    auto children_count = model.children_count(parent);

    auto lines_start_y = current_y_pos;
    float lines_end_y = 0;

    if (is_expanded(path)) {
        for (size_t r = 0; r < children_count; r++) {
            auto child_path = path;
            child_path.push_back(r);

            float current_x_pos = 0;

            auto child = model.child(parent, r);

            Gfx::RectangleDrawOptions line_rect;
            line_rect.fill_color = theme().placeholder;
            Util::Vector2f line_position { depth * IndentSize - IndentSize / 2, current_y_pos + row_height / 2.f };
            line_position += scroll_offset();
            painter.draw_rectangle({ line_position, { IndentSize / 2, 1 } }, line_rect);

            float first_column_position = depth * IndentSize;
            if (model.children_count(&child) > 0) {
                auto base_position = Util::Vector2f { first_column_position + 13, current_y_pos + row_height / 2.f } + scroll_offset();

                std::vector<Util::Vector2f> vertices;
                if (is_expanded(child_path)) {
                    vertices.push_back(base_position + Util::Vector2f { -6, -3 });
                    vertices.push_back(base_position + Util::Vector2f { 0, 3 });
                    vertices.push_back(base_position + Util::Vector2f { 6, -3 });
                }
                else {
                    vertices.push_back(base_position + Util::Vector2f { -3, -6 });
                    vertices.push_back(base_position + Util::Vector2f { 3, 0 });
                    vertices.push_back(base_position + Util::Vector2f { -3, 6 });
                }
                painter.draw_line(vertices, Gfx::LineDrawOptions { .color = theme().text_button.active.unhovered.text });
                first_column_position += 22;
            }

            auto icon = model.icon(child);
            if (icon) {
                Gfx::RectangleDrawOptions rect;
                rect.texture = icon;
                Util::Vector2f icon_position { first_column_position + 4, current_y_pos + row_height / 2.f - 8 };
                icon_position += scroll_offset();
                painter.draw_rectangle({ icon_position, { 16, 16 } }, rect);
                first_column_position += 20;
            }

            for (size_t c = 0; c < columns; c++) {
                auto column = model.column(c);

                auto data = model.data(child, c);
                Util::Vector2f cell_position { c == 0 ? first_column_position : current_x_pos, current_y_pos };
                cell_position += scroll_offset();
                auto cell_size = this->cell_size(r, c);

                // TODO: ClipViewScope it
                // TODO: Make this all (font, font raw_size, alignment) configurable
                std::visit(
                    Util::Overloaded {
                        [&](Util::UString const& data) {
                            Gfx::Text text { data, Application::the().bold_font() };
                            text.set_font_size(theme().label_font_size);
                            text.set_fill_color(c % 2 == 0 ? list_even.text : list_odd.text);
                            text.align(Align::CenterLeft, { cell_position + Util::Vector2f(5, 0), cell_size });
                            text.draw(painter);
                        },
                        [&](llgl::Texture const* data) {
                            Gfx::RectangleDrawOptions rect;
                            rect.texture = data;
                            painter.draw_rectangle({ { cell_position.x() + cell_size.x() / 2 - 8, cell_position.y() + cell_size.y() / 2 - 8 }, { 16, 16 } }, rect);
                        } },
                    data);

                current_x_pos += cell_size.x();
            }

            current_y_pos += row_height;

            lines_end_y = current_y_pos - row_height / 2.f;
            render_rows(painter, current_y_pos, child_path, &child);
        }

        if (children_count > 0) {
            Gfx::RectangleDrawOptions line_rect;
            line_rect.fill_color = theme().placeholder;
            Util::Vector2f line_position { depth * IndentSize - IndentSize / 2, lines_start_y };
            line_position += scroll_offset();
            painter.draw_rectangle({ line_position, { 1, lines_end_y - lines_start_y + 1 } }, line_rect);
        }
    }
}

size_t TreeView::displayed_row_count() const {
    return recursive_displayed_row_count(nullptr, {});
}

size_t TreeView::recursive_displayed_row_count(Model::Node const* node, std::vector<size_t> path) const {
    size_t children_count = model().children_count(node);
    size_t total_count = children_count;
    for (size_t s = 0; s < children_count; s++) {
        auto child = model().child(node, s);
        auto child_path = path;
        child_path.push_back(s);
        total_count += is_expanded(child_path)
            ? recursive_displayed_row_count(&child, std::move(child_path))
            : 0;
    }
    return total_count;
}

std::pair<std::vector<size_t>, Model::Node> TreeView::displayed_row_at_index(size_t row) const {
    row += 1;
    return recursive_displayed_row_at_index(nullptr, {}, row);
}

std::pair<std::vector<size_t>, Model::Node> TreeView::recursive_displayed_row_at_index(Model::Node const* parent, std::vector<size_t> path, size_t& index) const {
    if (!is_expanded(path)) {
        return {};
    }
    auto children_count = model().children_count(parent);
    for (size_t s = 0; s < children_count; s++) {
        auto child_path = path;
        child_path.push_back(s);
        auto child = model().child(parent, s);
        index--;
        if (index == 0) {
            return { child_path, child };
        }
        auto node = recursive_displayed_row_at_index(&child, std::move(child_path), index);
        if (index == 0) {
            return node;
        }
    }
    return {};
}

Util::Vector2f TreeView::content_size() const {
    return { row_width(), (display_header() ? displayed_row_count() + 1 : displayed_row_count()) * theme().line_height };
}

bool TreeView::is_expanded(std::vector<size_t> const& path) const {
    return m_expanded_paths.contains(path);
}

}