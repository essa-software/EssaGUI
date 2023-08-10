#include "Splitter.hpp"

#include <Essa/GUI/Graphics/Drawing/Fill.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>

#include <iterator>
#include <ranges>

namespace GUI {

constexpr unsigned SplitterWidth = 5;

Widget::EventHandlerResult Splitter::on_mouse_move(Event::MouseMove const& event) {
    if (m_splitter_dragged) {
        int position = event.local_position().main(m_orientation);
        auto delta = position - m_drag_start;
        auto& first_widget = m_widgets[*m_splitter_dragged];
        auto& second_widget = m_widgets[*m_splitter_dragged + 1];
        auto first_size = first_widget->raw_size().main(m_orientation);
        auto second_size = second_widget->raw_size().main(m_orientation);

        int first_target_size = std::clamp(first_size + delta, 1, first_size + second_size - 1);
        int second_target_size = first_size + second_size - first_target_size;

        auto update_size = [this](Widget& widget, int target_size) {
            auto size = widget.size().main(m_orientation);
            if (size == Util::Length::Auto) {
                size = { 1, Util::Length::Auto };
            }
            switch (size.unit()) {
            case Util::Length::Initial:
                // Idk what to do here. Is this even reachable?
                ESSA_UNREACHABLE;
            case Util::Length::Px:
                size = { static_cast<float>(target_size), Util::Length::Px };
                break;
            case Util::Length::Auto: {
                int raw_size = std::max(1, widget.raw_size().main(m_orientation));
                size = { static_cast<float>(target_size) * size.value() / static_cast<float>(raw_size), Util::Length::Auto };
                break;
            }
            case Util::Length::Percent: {
                int raw_size = std::max(1, widget.raw_size().main(m_orientation));
                size = { static_cast<float>(target_size) * size.value() / static_cast<float>(raw_size), Util::Length::Percent };
                break;
            }
            }
            widget.set_size(LengthVector::from_main_cross(m_orientation, size, widget.size().cross(m_orientation)));
        };

        update_size(*first_widget, first_target_size);
        update_size(*second_widget, second_target_size);

        m_drag_start = first_widget->parent_relative_rect().position().main(m_orientation) + first_target_size;
        relayout();
    }
    return EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult Splitter::on_mouse_button_press(Event::MouseButtonPress const& event) {
    if (event.button() == llgl::MouseButton::Left) {
        std::optional<size_t> splitter;
        for (size_t i = 0; i + 1 < m_widgets.size(); i++) {
            if (splitter_rect(i).contains(event.local_position().cast<unsigned>())) {
                splitter = i;
            }
        }
        if (splitter) {
            m_splitter_dragged = splitter;
            m_drag_start = event.local_position().main(m_orientation);
            return EventHandlerResult::Accepted;
        }
    }
    return EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult Splitter::on_mouse_button_release(Event::MouseButtonRelease const& event) {
    if (event.button() == llgl::MouseButton::Left) {
        m_splitter_dragged = {};
    }
    return EventHandlerResult::NotAccepted;
}

void Splitter::draw(Gfx::Painter& painter) const {
    for (size_t i = 0; i + 1 < m_widgets.size(); i++) {
        Util::Rectu rect = splitter_rect(i);
        using namespace Gfx::Drawing;
        painter.draw(Rectangle(rect.cast<float>(), Fill::solid(theme().sidebar)));
    }
}

Util::Rectu Splitter::splitter_rect(size_t index) const {
    assert(index + 1 < m_widgets.size());
    auto const& widget = m_widgets[index + 1];
    auto position = widget->parent_relative_rect().position();

    return {
        Util::Point2u::from_main_cross(m_orientation, position.main(m_orientation) - SplitterWidth, 0),
        Util::Size2u::from_main_cross(m_orientation, SplitterWidth, m_raw_size.cross(m_orientation)),
    };
}

void Splitter::relayout() {
    if (m_widgets.empty()) {
        return;
    }

    BoxLayout layout(m_orientation);
    layout.set_spacing(SplitterWidth);
    layout.run(*this);

    // "Renormalize" fraction units to pixels to avoid precision loss over time
    for (auto& widget : m_widgets) {
        auto size = widget->size();
        if (size.main(m_orientation).unit() == Util::Length::Auto) {
            auto raw_size = widget->raw_size().main(m_orientation);
            widget->set_size(LengthVector::from_main_cross(
                m_orientation, { raw_size == 0 ? 1 : static_cast<float>(raw_size), Util::Length::Auto }, size.cross(m_orientation)
            ));
        }
    }
}

}
