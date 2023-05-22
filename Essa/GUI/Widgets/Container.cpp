#include <Essa/GUI/Widgets/Container.hpp>

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Graphics/Drawing/Outline.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
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
        switch (w->size().main(m_orientation).unit()) {
        case Util::Length::Px:
            // std::cout << "test" << std::endl;
            raw_size = w->size().main(m_orientation).value();
            break;
        case Util::Length::Percent:
            // std::cout << raw_size << std::endl;
            raw_size = w->size().main(m_orientation).value() * (container.raw_size().main(m_orientation) - total_spacing_size) / 100;
            break;
        case Util::Length::Auto:
            raw_size = 0;
            break;
        case Util::Length::Initial:
            ESSA_UNREACHABLE;
        }
        w->set_raw_size(Util::Size2i::from_main_cross(
            m_orientation, raw_size, container.raw_size().cross(m_orientation) - m_padding.cross_sum(m_orientation)
        ));
    }

    // 2. Compute raw_size available for auto-sized widgets
    int available_size_for_autosized_widgets = container.raw_size().main(m_orientation) - m_padding.main_sum(m_orientation);
    int autosized_widget_count = 0;
    for (auto& w : container.widgets()) {
        if (!w->is_visible())
            continue;
        if (w->size().main(m_orientation).unit() == Util::Length::Auto)
            autosized_widget_count++;
        else
            available_size_for_autosized_widgets -= w->raw_size().main(m_orientation) + m_spacing;
    }

    // 3. Set autosized widgets' raw_size + arrange widgets
    int autosized_widget_size = autosized_widget_count == 0
        ? 0
        : (available_size_for_autosized_widgets - (m_spacing * (autosized_widget_count - 1))) / autosized_widget_count;
    if (autosized_widget_size < 0)
        autosized_widget_size = 0;
    switch (m_content_alignment) {
    case ContentAlignment::BoxStart: {
        int current_position = 0;
        for (auto& w : container.widgets()) {
            if (!w->is_visible())
                continue;
            if (w->size().main(m_orientation).unit() == Util::Length::Auto) {
                w->set_raw_size(Util::Size2i::from_main_cross(
                    m_orientation, autosized_widget_size, container.raw_size().cross(m_orientation) - m_padding.cross_sum(m_orientation)
                ));
            }
            w->set_raw_position(Util::Point2i::from_main_cross(
                m_orientation, container.raw_position().main(m_orientation) + current_position + m_padding.main_start(m_orientation),
                container.raw_position().cross(m_orientation) + m_padding.cross_start(m_orientation)
            ));
            current_position += w->raw_size().main(m_orientation) + m_spacing;
            available_size_for_autosized_widgets -= autosized_widget_size;
        }
    } break;
    case ContentAlignment::BoxEnd: {
        int current_position = container.raw_size().main(m_orientation);
        for (auto it = container.widgets().rbegin(); it != container.widgets().rend(); it++) {
            auto& w = *it;
            if (!w->is_visible())
                continue;
            if (w->size().main(m_orientation).unit() == Util::Length::Auto) {
                w->set_raw_size(Util::Size2i::from_main_cross(
                    m_orientation, autosized_widget_size, container.raw_size().cross(m_orientation) - m_padding.cross_sum(m_orientation)
                ));
            }
            current_position -= w->raw_size().main(m_orientation) + m_spacing;
            w->set_raw_position(Util::Point2i::from_main_cross(
                m_orientation, container.raw_position().main(m_orientation) + current_position + m_padding.main_start(m_orientation),
                container.raw_position().cross(m_orientation) + m_padding.cross_start(m_orientation)
            ));
            available_size_for_autosized_widgets -= autosized_widget_size;
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

EML::EMLErrorOr<void> BoxLayout::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Layout::load_from_eml_object(object, loader));

    m_spacing = TRY(object.get_property("spacing", EML::Value(0.0)).to_double());
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
        auto input_position = w->position();
        w->set_raw_size({ resolve_size(container.raw_size().x(), w->size().x), resolve_size(container.raw_size().y(), w->size().y) });
        auto x = resolve_position(container.raw_size().x(), w->raw_size().x(), input_position.x);
        auto y = resolve_position(container.raw_size().y(), w->raw_size().y(), input_position.y);

        handle_alignment(x, w->raw_size().x(), container.raw_size().x(), w->horizontal_alignment());
        handle_alignment(y, w->raw_size().y(), container.raw_size().y(), w->vertical_alignment());

        w->set_raw_position({ x + container.raw_position().x(), y + container.raw_position().y() });
    }
}

Util::Size2i BasicLayout::total_size(Container const& container) const { return container.raw_size(); }

EML_REGISTER_CLASS(BasicLayout);

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
    for (size_t c = 0; auto& w : m_widgets) {
        if (widget_tree_root().focused_widget() == w.get())
            return c;
        if (recursive) {
            auto container = dynamic_cast<Container*>(w.get());
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
            if (m_parent && !steals_focus())
                m_parent->focus_next_widget(true);
            else
                focus_first_child_or_self();
            break;
        }
        else if (m_widgets[index]->is_visible() && m_widgets[index]->accepts_focus() && !m_widgets[index]->steals_focus()) {
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

void Container::do_update() {
    Widget::do_update();
    for (auto const& w : m_widgets)
        w->do_update();
}

void Container::relayout() {
    if (m_widgets.empty())
        return;
    for (auto& w : m_widgets) {
        auto initial_size = w->initial_size();
        if (w->m_input_size.x == Util::Length::Initial)
            w->m_input_size.x = initial_size.x;
        if (w->m_input_size.y == Util::Length::Initial)
            w->m_input_size.y = initial_size.y;
    }
    if (!m_layout) {
        std::cout << "Not trying to relayout widget without layout!" << std::endl;
        dump(0);
        assert(false);
    }
    if (m_layout->padding() == Boxi {})
        m_layout->set_padding(intrinsic_padding());
    m_layout->run(*this);
}

void Container::dump(unsigned depth) {
    Widget::dump(depth);
    ++depth;
    for (unsigned i = 0; i < depth; i++)
        std::cout << "-   ";
    if (m_layout) {
        std::cout << "layout: " << typeid(*m_layout).name() << std::endl;
    }
    else {
        std::cout << "layout: NONE!" << std::endl;
    }
    if (!is_visible())
        return;
    for (auto& w : m_widgets) {
        w->dump(depth);
    }
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

Util::Size2i Container::total_size() const { return m_layout->total_size(*this); }

EML::EMLErrorOr<void> Container::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_layout = TRY(object.require_and_construct_object<Layout>("layout", loader));

    for (auto const& child : object.objects) {
        std::shared_ptr<Widget> widget = TRY(child.construct<Widget>(loader, widget_tree_root()));
        assert(widget);
        add_created_widget(std::move(widget));
    }

    return {};
}

EML_REGISTER_CLASS(Container);

}
