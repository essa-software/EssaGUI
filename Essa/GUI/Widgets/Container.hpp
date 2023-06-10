#pragma once

#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Is.hpp>
#include <EssaUtil/Orientation.hpp>
#include <algorithm>
#include <initializer_list>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace GUI {

class Container;

using WidgetList = std::vector<std::shared_ptr<Widget>>;

template<class T> struct Box {
    T top {};
    T right {};
    T bottom {};
    T left {};

    static Box all_equal(T value) { return { value, value, value, value }; }

    T main_start(Util::Orientation o) const { return o == Util::Orientation::Horizontal ? left : top; }

    T main_end(Util::Orientation o) const { return o == Util::Orientation::Horizontal ? right : bottom; }

    T main_sum(Util::Orientation o) const { return o == Util::Orientation::Horizontal ? left + right : top + bottom; }

    T cross_start(Util::Orientation o) const { return o == Util::Orientation::Horizontal ? top : left; }

    T cross_end(Util::Orientation o) const { return o == Util::Orientation::Horizontal ? bottom : right; }

    T cross_sum(Util::Orientation o) const { return o == Util::Orientation::Horizontal ? top + bottom : left + right; }

    bool operator==(Box<T> const& other) const = default;
};
using Boxi = Box<int>;

class Layout : public EML::EMLObject {
public:
    Layout() = default;
    Layout(Layout const&) = delete;
    Layout& operator=(Layout const&) = delete;

    virtual ~Layout() = default;

    virtual void run(Container&) = 0;
    virtual Util::Size2i total_size(Container const&) const = 0;

    CREATE_VALUE(Boxi, padding, Boxi {})

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
};

/// Widgets are resized to fill up the entire space (in the vertical axis)
class BoxLayout : public Layout {
public:
    BoxLayout(Util::Orientation o)
        : m_orientation(o) { }

    // Spacing = a gap between widgets (but not between edges and widgets)
    CREATE_VALUE(int, spacing, 0)
    virtual void run(Container&) override;
    virtual Util::Size2i total_size(Container const&) const override;

    enum class ContentAlignment { BoxStart, BoxEnd };

    CREATE_VALUE(ContentAlignment, content_alignment, ContentAlignment::BoxStart)

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    Util::Orientation m_orientation;
};

class VerticalBoxLayout : public BoxLayout {
public:
    VerticalBoxLayout()
        : BoxLayout(Util::Orientation::Vertical) { }
};

class HorizontalBoxLayout : public BoxLayout {
public:
    HorizontalBoxLayout()
        : BoxLayout(Util::Orientation::Horizontal) { }
};

// Just assigns input_size to size.
class BasicLayout : public Layout {
private:
    virtual void run(Container&) override;
    virtual Util::Size2i total_size(Container const&) const override;
};

class Container : public Widget {
public:
    template<class T, class... Args>
        requires(std::is_base_of_v<Widget, T> && requires(Args && ... args) { T(std::forward<Args>(args)...); })
    T* add_widget(Args&&... args) {
        auto widget = std::make_shared<T>(std::forward<Args>(args)...);
        m_widgets.push_back(widget);
        widget->set_parent(*this);
        widget->init();
        set_needs_relayout();
        return widget.get();
    }

    void add_created_widget(std::shared_ptr<Widget> widget) {
        widget->set_parent(*this);
        widget->init();
        m_widgets.push_back(std::move(widget));
        set_needs_relayout();
    }

    void remove_widget(size_t index);

    virtual void do_update() override;
    virtual Widget::EventHandlerResult do_handle_event(Event const&) override;
    virtual void do_relayout() override;
    virtual void do_draw(Gfx::Painter& window) const override;

    virtual bool is_focused() const override;

    void shrink(size_t num) { m_widgets.resize(std::min(num, m_widgets.size())); }

    template<class T, class... Args>
        requires(std::is_base_of_v<Layout, T> && requires(Args && ... args) { T(args...); })
    T& set_layout(Args&&... args) {
        auto layout = std::make_unique<T>(std::forward<Args>(args)...);
        auto layout_ptr = layout.get();
        m_layout = std::move(layout);
        return *layout_ptr;
    }

    void clear_layout() { m_layout = nullptr; }
    std::unique_ptr<Layout>& get_layout() { return m_layout; }

    virtual void dump(unsigned depth) override;

    Widget* find_widget_by_id(std::string_view) const;
    std::vector<Widget*> find_widgets_by_class_name(std::string_view) const;
    std::vector<Widget*> find_widgets_by_class_name_recursively(std::string_view) const;
    Widget* find_widget_by_id_recursively(std::string_view) const;

    // clang-format off
    // It behaves very badly when encounters 'requires'.
    template<class T>
    requires(std::is_base_of_v<Widget, T>)
    T* find_widget_of_type_by_id(std::string_view name) const {
        return dynamic_cast<T*>(find_widget_by_id(name));
    }

    template<class T>
    requires(std::is_base_of_v<Widget, T>)
    T* find_widget_of_type_by_id_recursively(std::string_view name) const {
        return dynamic_cast<T*>(find_widget_by_id_recursively(name));
    }

    template<class T>
    requires(std::is_base_of_v<Widget, T>)
    T& find(std::string_view id) const {
        auto widget = find_widget_of_type_by_id_recursively<T>(id);
        assert(widget && Util::is<T>(widget));
        return *widget;
    }

    template<class T>
    requires(std::is_base_of_v<Widget, T>)
    std::vector<T*> find_widgets_of_type_by_class_name(std::string_view name) const {
        auto elements = find_widgets_by_class_name(name);
        std::vector<T*> result;
        for (unsigned i = 0; i < elements.size(); i++) {
            auto maybe_element = dynamic_cast<T*>(elements[i]);
            if (maybe_element)
                result.push_back(maybe_element);
        }
            
        return result;
    }

    template<class T>
    requires(std::is_base_of_v<Widget, T>)
    std::vector<T*> find_widgets_of_type_by_class_name_recursively(std::string_view name) const {
        auto elements = find_widgets_by_class_name_recursively(name);
        std::vector<T*> result;
        for (unsigned i = 0; i < elements.size(); i++) {
            auto maybe_element = dynamic_cast<T*>(elements[i]);
            if (maybe_element)
                result.push_back(maybe_element);
        }

        return result;
    }
    // clang-format on

    // Footgun alert, use it only in *Layout::run()!
    WidgetList& widgets() { return m_widgets; }
    WidgetList const& widgets() const { return m_widgets; }

protected:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual void relayout() override;
    virtual Boxi intrinsic_padding() const { return {}; }
    virtual void focus_first_child_or_self() override;
    virtual bool accepts_focus() const override;

    virtual EventHandlerResult on_key_press(Event::KeyPress const&) override;

    std::optional<size_t> focused_widget_index(bool recursive) const;
    // Returns true if the focus changed (one of children was focused)
    bool focus_next_widget(bool called_from_child);
    void focus_first_child();

    WidgetList m_widgets;

private:
    friend Layout;
    void m_find_widgets_by_class_name_recursively_helper(std::string_view class_name, std::vector<Widget*>& vec) const;

    virtual Util::Size2i total_size() const override;

    std::unique_ptr<Layout> m_layout;
};

}
