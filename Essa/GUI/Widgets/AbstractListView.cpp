#include "AbstractListView.hpp"

#include <Essa/GUI/Widgets/ScrollableWidget.hpp>

namespace GUI {

Util::Vector2f AbstractListView::row_position(size_t row) const {
    return Util::Vector2f { raw_position().x(), raw_position().y() + theme().line_height * (display_header() ? row + 1 : row) } + scroll_offset();
}

Util::Vector2f AbstractListView::content_size() const {
    return { row_width(), (display_header() ? m_model->row_count() + 1 : m_model->row_count()) * theme().line_height };
}

float AbstractListView::row_width() const {
    float width = 0;
    for (size_t c = 0; c < m_model->column_count(); c++) {
        width += m_model->column(c).width;
    }
    return width;
}

Util::Vector2f AbstractListView::cell_size(size_t, size_t column) const {
    return { m_model->column(column).width, theme().line_height };
}

EML::EMLErrorOr<void> AbstractListView::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(ScrollableWidget::load_from_eml_object(object, loader));

    m_display_header = TRY(object.get_property("display_header", false).to_bool());
    return {};
}

}
