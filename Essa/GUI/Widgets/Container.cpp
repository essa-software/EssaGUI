#include <Essa/GUI/Widgets/Container.hpp>

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Drawing/Outline.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/WindowRoot.hpp>
#include <Essa/LLGL/Window/Event.hpp>
#include <EssaUtil/Config.hpp>
#include <cassert>
#include <iostream>
#include <vector>

namespace GUI {

EML::EMLErrorOr<void> Layout::load_from_eml_object(EML::Object const& object, EML::Loader&) {
    auto padding = object.get_property("padding", EML::Value(0.0));
    if (padding.is_array()) {
        auto padding_array = TRY(padding.as_array().to_static<double, 4>());
        fmt::print("padding: {}\n", fmt::join(padding_array, ", "));
        m_padding = Boxi { static_cast<int>(padding_array[0]), static_cast<int>(padding_array[1]), static_cast<int>(padding_array[2]),
                           static_cast<int>(padding_array[3]) };
    }
    else if (padding.is_double()) {
        auto padding_double = padding.as_double();
        m_padding = Boxi { static_cast<int>(padding_double), static_cast<int>(padding_double), static_cast<int>(padding_double),
                           static_cast<int>(padding_double) };
    }

    return {};
}

void BoxLayout::run(Container& container) {
    int total_spacing_size = (m_spacing * static_cast<int>(container.widgets().size() - 1));

    // 1. Compute widget raw_size (in main axis) if it has fixed raw_size
    for (auto& w : container.widgets()) {
        if (!w->is_visible())
            continue;

        int raw_size = 0;
        switch (w->input_size().main(m_orientation).unit()) {
        case Util::Length::Px:
            // std::cout << "test" << std::endl;
            raw_size = w->input_size().main(m_orientation).value();
            break;
        case Util::Length::Percent:
            // std::cout << raw_size << std::endl;
            raw_size = w->input_size().main(m_orientation).value() * (container.raw_size().main(m_orientation) - total_spacing_size) / 100;
            break;
        case Util::Length::Auto:
            raw_size = 0;
            break;
        case Util::Length::Initial:
            ESSA_UNREACHABLE;
        }
        int raw_cross_size = [&]() {
            auto size = w->input_size().cross(m_orientation);
            switch (size.unit()) {
            case Util::Length::Auto:
                return container.raw_size().cross(m_orientation) - m_padding.cross_sum(m_orientation);
            case Util::Length::Px:
                return static_cast<int>(size.value());
            case Util::Length::Percent:
                return static_cast<int>(size.value() * container.raw_size().cross(m_orientation) / 100);
            case Util::Length::Initial:
                break;
            }
            ESSA_UNREACHABLE;
        }();
        w->set_raw_size(Util::Size2i::from_main_cross(m_orientation, raw_size, raw_cross_size));
    }

    // 2. Compute raw_size available for auto-sized widgets
    int available_size_for_autosized_widgets = container.raw_size().main(m_orientation) - m_padding.main_sum(m_orientation);
    float autosized_fraction_sum = 0;
    int autosized_widget_count = 0;
    for (auto& w : container.widgets()) {
        if (!w->is_visible())
            continue;
        if (w->input_size().main(m_orientation).unit() == Util::Length::Auto) {
            autosized_fraction_sum += w->input_size().main(m_orientation).value();
            autosized_widget_count++;
        }
        else {
            available_size_for_autosized_widgets -= w->raw_size().main(m_orientation) + m_spacing;
        }
    }

    // 3. Set autosized widgets' raw_size + arrange widgets
    float autosized_widget_size = autosized_fraction_sum == 0
        ? 0
        : (static_cast<float>(available_size_for_autosized_widgets) - (static_cast<float>(m_spacing * (autosized_widget_count - 1))))
            / autosized_fraction_sum;

    if (autosized_widget_size < 0)
        autosized_widget_size = 0;
    switch (m_content_alignment) {
    case ContentAlignment::BoxStart: {
        int current_position = 0;
        for (auto& w : container.widgets()) {
            if (!w->is_visible())
                continue;
            if (w->input_size().main(m_orientation).unit() == Util::Length::Auto) {
                auto cross = w->raw_size().cross(m_orientation);
                w->set_raw_size(Util::Size2i::from_main_cross(
                    m_orientation, std::ceil(autosized_widget_size * w->input_size().main(m_orientation).value()), cross
                ));
            }
            w->set_raw_position(Util::Point2i::from_main_cross(
                m_orientation, container.raw_position().main(m_orientation) + current_position + m_padding.main_start(m_orientation),
                container.raw_position().cross(m_orientation) + m_padding.cross_start(m_orientation)
            ));
            current_position += w->raw_size().main(m_orientation) + m_spacing;
        }
    } break;
    case ContentAlignment::BoxEnd: {
        int current_position = container.raw_size().main(m_orientation);
        for (auto it = container.widgets().rbegin(); it != container.widgets().rend(); it++) {
            auto& w = *it;
            if (!w->is_visible())
                continue;
            if (w->input_size().main(m_orientation).unit() == Util::Length::Auto) {
                auto cross = w->raw_size().cross(m_orientation);
                w->set_raw_size(Util::Size2i::from_main_cross(
                    m_orientation, std::ceil(autosized_widget_size * w->input_size().main(m_orientation).value()), cross
                ));
            }
            current_position -= w->raw_size().main(m_orientation);
            w->set_raw_position(Util::Point2i::from_main_cross(
                m_orientation, container.raw_position().main(m_orientation) + current_position + m_padding.main_start(m_orientation),
                container.raw_position().cross(m_orientation) + m_padding.cross_start(m_orientation)
            ));
            current_position -= m_spacing;
        }
    } break;
    }

    /*for(auto& w : widgets())
    {
        std::cout << w->size().x() << "," << w->size().y() << " @ " <<
    w->input_position().x() << "," << w->input_position().y() << " ----> ";
        std::cout << w->raw_size().x() << "," << w->raw_size().y() << " @ " <<
    w->position().x() << "," << w->position().y() << std::endl;
    }*/
}

Util::Size2i BoxLayout::total_size(Container const& container) const {
    int main_size = m_padding.main_sum(m_orientation);
    for (auto const& widget : container.widgets()) {
        main_size += widget->raw_size().main(m_orientation) + m_spacing;
    }
    main_size -= m_spacing;
    int cross_size = container.raw_size().cross(m_orientation);
    return Util::Size2i::from_main_cross(m_orientation, main_size, cross_size);
}

LengthVector BoxLayout::initial_size(Container const& container) const {
    if (container.widgets().empty()) {
        return LengthVector { 0_px, 0_px };
    }
    float main_total_size = static_cast<float>(m_padding.main_sum(m_orientation) + m_spacing * (container.widgets().size() - 1));
    bool forward_auto_size_for_main_axis = false;
    float cross_total_size = 0;
    bool forward_auto_size_for_cross_axis = false;
    for (auto const& widget : container.widgets()) {
        auto widget_size = widget->input_size();
        auto main_size = widget_size.main(m_orientation);
        auto cross_size = widget_size.cross(m_orientation);
        switch (main_size.unit()) {
        case Util::Length::Initial:
            ESSA_UNREACHABLE;
        case Util::Length::Auto:
            forward_auto_size_for_main_axis = true;
            break;
        case Util::Length::Px:
            main_total_size += main_size.value();
            break;
        case Util::Length::Percent:
            main_total_size += main_size.value() / 100.f * static_cast<float>(container.raw_size().main(m_orientation));
            break;
        }
        switch (cross_size.unit()) {
        case Util::Length::Initial:
            ESSA_UNREACHABLE;
        case Util::Length::Auto:
            forward_auto_size_for_cross_axis = true;
            break;
        case Util::Length::Px:
            cross_total_size = std::max(cross_total_size, cross_size.value());
            break;
        case Util::Length::Percent:
            cross_total_size
                = std::max(cross_total_size, cross_size.value() / 100.f * static_cast<float>(container.raw_size().cross(m_orientation)));
            break;
        }
    }

    return LengthVector::from_main_cross(
        m_orientation, forward_auto_size_for_main_axis ? Util::Length::Auto : Util::Length { main_total_size, Util::Length::Px },
        forward_auto_size_for_cross_axis
            ? Util::Length::Auto
            : Util::Length { cross_total_size + static_cast<float>(m_padding.cross_sum(m_orientation)), Util::Length::Px }
    );
}

EML::EMLErrorOr<void> BoxLayout::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Layout::load_from_eml_object(object, loader));

    m_spacing = TRY(object.get_property("spacing", EML::Value(0.0)).to_double());
    m_content_alignment = TRY(object.get_enum("content_alignment", content_alignment_from_string, ContentAlignment::BoxStart));
    return {};
}

EML_REGISTER_CLASS(HorizontalBoxLayout);
EML_REGISTER_CLASS(VerticalBoxLayout);

void BasicLayout::run(Container& container) {
    auto resolve_position = [&](int container_size, int widget_size, Util::Length const& input_position) -> int {
        switch (input_position.unit()) {
        case Util::Length::Px:
            return input_position.value();
        case Util::Length::Percent:
            return widget_size * container_size / 100;
        default:
            return 0;
        }
    };

    auto resolve_size = [&](int container_size, Util::Length const& size) -> int {
        switch (size.unit()) {
        case Util::Length::Px:
            return size.value();
        case Util::Length::Percent:
            return size.value() * container_size / 100;
        default:
            return 0;
        }
    };

    auto handle_alignment = [](int& v, int child_size, int container_size, Widget::Alignment alignment) {
        switch (alignment) {
        case Widget::Alignment::Start:
            break;
        case Widget::Alignment::Center:
            v += container_size / 2;
            v -= child_size / 2;
            break;
        case Widget::Alignment::End:
            v = container_size - v - child_size;
            break;
        }
    };

    for (auto& w : container.widgets()) {
        auto input_position = w->input_position();
        w->set_raw_size({ resolve_size(container.raw_size().x(), w->input_size().x),
                          resolve_size(container.raw_size().y(), w->input_size().y) });
        auto x = resolve_position(container.raw_size().x(), w->raw_size().x(), input_position.x);
        auto y = resolve_position(container.raw_size().y(), w->raw_size().y(), input_position.y);

        handle_alignment(x, w->raw_size().x(), container.raw_size().x(), w->horizontal_alignment());
        handle_alignment(y, w->raw_size().y(), container.raw_size().y(), w->vertical_alignment());

        w->set_raw_position({ x + container.raw_position().x(), y + container.raw_position().y() });
    }
}

Util::Size2i BasicLayout::total_size(Container const& container) const {
    return container.raw_size();
}
LengthVector BasicLayout::initial_size(Container const&) const {
    return { Util::Length::Auto, Util::Length::Auto };
}

EML_REGISTER_CLASS(BasicLayout);

void Container::remove_widget(size_t index) {
    assert(index < m_widgets.size());
    m_widgets.erase(m_widgets.begin() + index);
    set_needs_relayout();
}

void Container::remove_widget(Widget& widget) {
    auto it = std::find_if(m_widgets.begin(), m_widgets.end(), [&](auto const& w) { return w.get() == &widget; });
    assert(it != m_widgets.end());
    m_widgets.erase(it);
    set_needs_relayout();
}

LengthVector Container::initial_size() const {
    for (auto const& widget : m_widgets) {
        widget->copy_initial_sizes();
    }
    return m_layout ? m_layout->initial_size(*this) : LengthVector {};
}

void Container::do_relayout() {
    Widget::do_relayout();
    for (auto const& w : m_widgets) {
        if (w->is_visible())
            w->do_relayout();
    }
}

// Defined in Widget.cpp
DBG_DECLARE_EXTERN(GUI_DrawWidgetLayoutBounds);

void Container::do_draw(Gfx::Painter& painter) const {
    // FIXME: This code is duplicated from Widget, but we need
    //        a CVS active during rendering of all the children.
    Gfx::ClipViewScope scope(painter, parent_relative_rect(), Gfx::ClipViewScope::Mode::Intersect);

    Gfx::RectangleDrawOptions background;
    background.fill_color = m_background_color;
    painter.deprecated_draw_rectangle(local_rect().cast<float>(), background);

    this->draw(painter);

    if (DBG_ENABLED(GUI_DrawWidgetLayoutBounds)) {
        using namespace Gfx::Drawing;
        painter.draw(Rectangle(local_rect().cast<float>(), Fill::none(), Outline::normal(Util::Colors::Green, -1)));
    }

    for (auto const& w : m_widgets) {
        if (w->is_visible())
            w->do_draw(painter);
    }
}

Widget::EventHandlerResult Container::do_handle_event(Event const& event) {
    for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); it++) {
        auto& widget = *it;
        if (!widget->is_visible())
            continue;

        if (widget->do_handle_event(event) == EventHandlerResult::Accepted) {
            return EventHandlerResult::Accepted;
        }
    }

    // Bubble the event to current Container.
    return Widget::do_handle_event(event);
}

Widget::EventHandlerResult Container::on_key_press(Event::KeyPress const& event) {
    if (event.code() == llgl::KeyCode::Tab) {
        if (focus_next_widget(false)) {
            return EventHandlerResult::Accepted;
        }
    }
    return EventHandlerResult::NotAccepted;
}

bool Container::is_focused() const {
    for (auto const& child : m_widgets) {
        if (child->is_focused()) {
            return true;
        }
    }
    return Widget::is_focused();
}

std::optional<size_t> Container::focused_widget_index(bool recursive) const {
    for (size_t c = 0; auto const& w : m_widgets) {
        if (window_root().focused_widget() == w.get())
            return c;
        if (recursive) {
            auto* container = dynamic_cast<Container*>(w.get());
            if (container) {
                auto index = container->focused_widget_index(true);
                if (index.has_value())
                    return c;
            }
        }
        c++;
    }
    return {};
}

bool Container::focus_next_widget(bool called_from_child) {
    auto index_opt = focused_widget_index(called_from_child);
    if (!index_opt.has_value())
        return false;
    // if (!called_from_child)
    //     std::cout << "======================RUNNING TAB
    //     FOCUS======================" << std::endl;
    auto index = index_opt.value();

    if (m_widgets[index]->steals_focus())
        return false;

    do {
        index++;
        // dump(0);
        // std::cout << "focus_next_widget " << called_from_child << ": " <<
        // index << " " << m_widgets.raw_size() << std::endl;
        if (index == m_widgets.size()) {
            if (m_parent && !steals_focus() && Util::is<Container>(*m_parent)) {
                static_cast<Container*>(m_parent)->focus_next_widget(true);
            }
            else {
                focus_first_child_or_self();
            }
            break;
        }
        if (m_widgets[index]->is_visible() && m_widgets[index]->accepts_focus() && !m_widgets[index]->steals_focus()) {
            // std::cout << "Focusing first_child_or_self: " << index <<
            // std::endl;
            m_widgets[index]->focus_first_child_or_self();
            break;
        }
    } while (true);
    return true;
}

void Container::focus_first_child_or_self() {
    if (m_widgets.size() == 0) {
        std::cout << "No widgets to focus @" << this << std::endl;
        return;
    }

    for (auto& widget : m_widgets) {
        if (widget->is_visible() && widget->accepts_focus() && !widget->steals_focus()) {
            widget->focus_first_child_or_self();
            break;
        }
    }
}

bool Container::accepts_focus() const {
    if (steals_focus())
        return false;
    for (auto& widget : m_widgets) {
        if (widget->is_visible() && widget->accepts_focus())
            return true;
    }
    return false;
}

llgl::Cursor const& Container::cursor(Util::Point2i local_position) const {
    for (auto const& widget : m_widgets) {
        if (widget->is_visible() && widget->is_mouse_over(local_position + absolute_position().to_vector())) {
            return widget->cursor(local_position - widget->parent_relative_position().to_vector());
        }
    }
    return Widget::cursor(local_position);
}

void Container::do_update() {
    Widget::do_update();
    for (auto const& w : m_widgets)
        w->do_update();
}

void Container::relayout() {
    if (m_widgets.empty())
        return;
    if (!m_layout) {
        std::cout << "Not trying to relayout widget without layout!" << std::endl;
        dump(std::cerr, 0);
        assert(false);
    }
    if (m_layout->padding() == Boxi {})
        m_layout->set_padding(intrinsic_padding());
    for (auto const& widget : m_widgets) {
        widget->copy_initial_sizes();
    }
    m_layout->run(*this);
}

void Container::dump(std::ostream& out, unsigned depth) {
    Widget::dump(out, depth);
    ++depth;
    for (unsigned i = 0; i < depth; i++)
        out << "-   ";
    if (m_layout) {
        out << "layout: " << typeid(*m_layout).name() << std::endl;
    }
    else {
        out << "layout: NONE!" << std::endl;
    }
    if (!is_visible())
        return;
    for (auto& w : m_widgets) {
        w->dump(out, depth);
    }
}

std::vector<DevToolsObject const*> Container::dev_tools_children() const {
    std::vector<DevToolsObject const*> vec;
    for (auto const& w : m_widgets) {
        vec.push_back(w.get());
    }
    return vec;
}

Widget* Container::find_widget_by_id(std::string_view id) const {
    // FIXME: Make it not O(n)
    for (auto& w : m_widgets) {
        if (w->id() == id)
            return w.get();
    }
    return nullptr;
}

std::vector<Widget*> Container::find_widgets_by_class_name(std::string_view class_name) const {
    std::vector<Widget*> result;
    for (auto& w : m_widgets) {
        if (w->class_name() == class_name)
            result.push_back(w.get());
    }
    return result;
}

void Container::m_find_widgets_by_class_name_recursively_helper(std::string_view class_name, std::vector<Widget*>& vec) const {
    auto widget_vec = find_widgets_by_class_name(class_name);
    for (auto& w : widget_vec)
        vec.push_back(w);

    for (auto& w : m_widgets) {
        auto container = dynamic_cast<Container*>(w.get());
        if (container) {
            container->m_find_widgets_by_class_name_recursively_helper(class_name, vec);
        }
    }
}

std::vector<Widget*> Container::find_widgets_by_class_name_recursively(std::string_view class_name) const {
    std::vector<Widget*> result;
    m_find_widgets_by_class_name_recursively_helper(class_name, result);

    return result;
}

Widget* Container::find_widget_by_id_recursively(std::string_view id) const {
    auto widget = find_widget_by_id(id);
    if (widget)
        return widget;
    for (auto& w : m_widgets) {
        auto container = dynamic_cast<Container*>(w.get());
        if (container) {
            widget = container->find_widget_by_id_recursively(id);
            if (widget)
                return widget;
        }
    }
    return nullptr;
}

Util::Size2i Container::total_size() const {
    return m_layout->total_size(*this);
}

EML::EMLErrorOr<void> Container::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_layout = TRY(object.require_and_construct_object<Layout>("layout", loader));

    for (auto const& child : object.objects) {
        std::shared_ptr<Widget> widget = TRY(child.construct<Widget>(loader, window_root()));
        assert(widget);
        add_created_widget(std::move(widget));
    }

    return {};
}

EML_REGISTER_CLASS(Container);

}
