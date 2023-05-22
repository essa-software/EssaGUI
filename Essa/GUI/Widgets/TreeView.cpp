#include "TreeView.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Widgets/AbstractListView.hpp>
#include <Essa/GUI/Widgets/ScrollableWidget.hpp>
#include <Essa/LLGL/Window/Mouse.hpp>
#include <EssaUtil/Vector.hpp>

namespace GUI {

constexpr float IndentSize = 24;

Widget::EventHandlerResult TreeView::on_mouse_button_press(
    Event::MouseButtonPress const& event) {
    if (event.button() == llgl::MouseButton::Left) {
        size_t row = (event.local_position().y() - scroll_offset().y())
            / theme().line_height;
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
    return EventHandlerResult::NotAccepted;
}

void TreeView::draw(Gfx::Painter& wnd) const {
    if (!model()) {
        return;
    }
    auto& model = *this->model();

    auto row_height = theme().line_height;
    auto row_width = this->row_width();

    size_t rows = displayed_row_count();
    size_t columns = model.column_count();

    size_t first_row = -scroll_offset().y() / row_height;
    if (first_row > 0)
        first_row--;
    size_t last_row = std::min<size_t>(
        rows, (-scroll_offset().y() + scroll_area_size().y()) / row_height + 1);
    if (last_row < rows - 1)
        last_row++;

    auto list_odd = theme().list_odd;
    auto list_even = theme().list_even;

    // fmt::print("rows = {}/ render: {}..{}\n", rows, first_row, last_row);

    // Background
    if (rows > 0) {
        for (size_t r = first_row; r < last_row; r++) {
            Gfx::RectangleDrawOptions rs;
            rs.fill_color
                = r % 2 == 0 ? list_even.background : list_odd.background;
            wnd.deprecated_draw_rectangle(
                {
                    Util::Cs::Point2f {
                        0, row_height * (display_header() ? r + 1 : r) }
                        + scroll_offset().cast<float>(),
                    { row_width, row_height },
                },
                rs);
        }
    }

    // Column names
    float current_y_pos = 0;
    if (display_header()) {
        Gfx::RectangleDrawOptions rs;
        rs.fill_color = theme().text_button.normal.unhovered.background;
        wnd.deprecated_draw_rectangle(
            { scroll_offset().cast<float>().to_point(),
                { row_width, row_height } },
            rs);

        float x_pos = 0;
        for (size_t c = 0; c < columns; c++) {
            auto column = model.column(c);

            Gfx::Text text { column.name, Application::the().bold_font() };
            text.set_font_size(16);
            text.set_position((Util::Cs::Point2f { x_pos + 5, 20 }
                + scroll_offset().cast<float>())
                                  .to_deprecated_vector());
            text.draw(wnd);

            x_pos += cell_size(0, c).x();
        }
        current_y_pos += row_height;
    }

    render_rows(wnd, current_y_pos, {}, {});

    ScrollableWidget::draw_scrollbar(wnd);

    theme().renderer().draw_text_editor_border(*this, false, wnd);
}

void TreeView::render_rows(Gfx::Painter& painter, float& current_y_pos,
    std::vector<size_t> path, std::optional<Model::Node> parent) const {
    auto& model = *this->model();
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

            auto child_data = model.child(parent, r);
            auto child = Model::Node { r, child_data };

            Gfx::RectangleDrawOptions line_rect;
            line_rect.fill_color = theme().placeholder;
            Util::Cs::Point2i line_position { depth * IndentSize
                    - IndentSize / 2,
                current_y_pos + row_height / 2.f };
            line_position += scroll_offset();
            painter.deprecated_draw_rectangle(
                { line_position.cast<float>(), { IndentSize / 2, 1 } },
                line_rect);

            float first_column_position = depth * IndentSize;
            if (model.children_count(child) > 0) {
                auto base_position = Util::Cs::Point2i {
                    first_column_position + 13, current_y_pos + row_height / 2.f
                } + scroll_offset();

                std::vector<Util::Cs::Point2f> vertices;
                if (is_expanded(child_path)) {
                    vertices.push_back(
                        (base_position + Util::Cs::Vector2i { -6, -3 })
                            .cast<float>());
                    vertices.push_back(
                        (base_position + Util::Cs::Vector2i { 0, 3 })
                            .cast<float>());
                    vertices.push_back(
                        (base_position + Util::Cs::Vector2i { 6, -3 })
                            .cast<float>());
                }
                else {
                    vertices.push_back(
                        (base_position + Util::Cs::Vector2i { -3, -6 })
                            .cast<float>());
                    vertices.push_back(
                        (base_position + Util::Cs::Vector2i { 3, 0 })
                            .cast<float>());
                    vertices.push_back(
                        (base_position + Util::Cs::Vector2i { -3, 6 })
                            .cast<float>());
                }
                painter.draw_line(vertices,
                    Gfx::LineDrawOptions {
                        .color = theme().text_button.active.unhovered.text });
                first_column_position += 22;
            }

            auto icon = model.icon(child);
            if (icon) {
                Gfx::RectangleDrawOptions rect;
                rect.texture = icon;
                Util::Cs::Point2i icon_position { first_column_position + 4,
                    current_y_pos + row_height / 2.f - 8 };
                icon_position += scroll_offset();
                painter.deprecated_draw_rectangle(
                    { icon_position.cast<float>(), { 16, 16 } }, rect);
                first_column_position += 20;
            }

            for (size_t c = 0; c < columns; c++) {
                auto column = model.column(c);

                auto data = model.data(child, c);
                Util::Cs::Point2i cell_position { c == 0 ? first_column_position
                                                         : current_x_pos,
                    current_y_pos };
                cell_position += scroll_offset();
                auto cell_size = this->cell_size(r, c);

                // TODO: ClipViewScope it
                // TODO: Make this all (font, font raw_size, alignment)
                // configurable
                std::visit(
                    Util::Overloaded {
                        [&](Util::UString const& data) {
                            Gfx::Text text { data,
                                Application::the().bold_font() };
                            text.set_font_size(theme().label_font_size);
                            text.set_fill_color(
                                c % 2 == 0 ? list_even.text : list_odd.text);
                            text.align(Align::CenterLeft,
                                { (cell_position + Util::Cs::Vector2i(5, 0))
                                        .cast<float>(),
                                    cell_size.cast<float>() });
                            text.draw(painter);
                        },
                        [&](Gfx::RichText const& data) {
                            Gfx::RichTextDrawable drawable { data,
                                {
                                    .default_font = Application::the().font(),
                                    .font_size
                                    = static_cast<int>(theme().label_font_size),
                                    .text_alignment = GUI::Align::CenterLeft,
                                } };
                            drawable.set_rect(
                                { (cell_position + Util::Cs::Vector2i(5, 0))
                                        .cast<float>(),
                                    cell_size.cast<float>() });
                            drawable.draw(painter);
                        },
                        [&](llgl::Texture const* data) {
                            Gfx::RectangleDrawOptions rect;
                            rect.texture = data;
                            painter.deprecated_draw_rectangle(
                                { { cell_position.x() + cell_size.x() / 2 - 8,
                                      cell_position.y() + cell_size.y() / 2
                                          - 8 },
                                    { 16, 16 } },
                                rect);
                        } },
                    data);

                current_x_pos += cell_size.x();
            }

            current_y_pos += row_height;

            lines_end_y = current_y_pos - row_height / 2.f;
            render_rows(painter, current_y_pos, child_path, child);
        }

        if (children_count > 0) {
            Gfx::RectangleDrawOptions line_rect;
            line_rect.fill_color = theme().placeholder;
            Util::Cs::Point2i line_position {
                depth * IndentSize - IndentSize / 2, lines_start_y
            };
            line_position += scroll_offset();
            painter.deprecated_draw_rectangle(
                { line_position.cast<float>(),
                    { 1, lines_end_y - lines_start_y + 1 } },
                line_rect);
        }
    }
}

size_t TreeView::displayed_row_count() const {
    return recursive_displayed_row_count({}, {});
}

size_t TreeView::recursive_displayed_row_count(
    std::optional<Model::Node> node, std::vector<size_t> path) const {

    if (!model()) {
        return 0;
    }
    size_t children_count = model()->children_count(node);
    size_t total_count = children_count;
    for (size_t s = 0; s < children_count; s++) {
        auto child = model()->child(node, s);
        auto child_path = path;
        child_path.push_back(s);
        total_count += is_expanded(child_path) ? recursive_displayed_row_count(
                           Model::Node { s, child }, std::move(child_path))
                                               : 0;
    }
    return total_count;
}

std::pair<std::vector<size_t>, Model::NodeData>
TreeView::displayed_row_at_index(size_t row) const {
    row += 1;
    return recursive_displayed_row_at_index({}, {}, row);
}

std::pair<std::vector<size_t>, Model::NodeData>
TreeView::recursive_displayed_row_at_index(std::optional<Model::Node> parent,
    std::vector<size_t> path, size_t& index) const {
    if (!model()) {
        return {};
    }
    if (!is_expanded(path)) {
        return {};
    }
    auto children_count = model()->children_count(parent);
    for (size_t s = 0; s < children_count; s++) {
        auto child_path = path;
        child_path.push_back(s);
        auto child = model()->child(parent, s);
        index--;
        if (index == 0) {
            return { child_path, child };
        }
        auto node = recursive_displayed_row_at_index(
            Model::Node { s, child }, std::move(child_path), index);
        if (index == 0) {
            return node;
        }
    }
    return {};
}

Util::Cs::Size2i TreeView::content_size() const {
    return { row_width(),
        (display_header() ? displayed_row_count() + 1 : displayed_row_count())
            * theme().line_height };
}

bool TreeView::is_expanded(std::vector<size_t> const& path) const {
    return m_expanded_paths.contains(path);
}

EML_REGISTER_CLASS(TreeView);

}
