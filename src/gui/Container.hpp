#pragma once

#include "Widget.hpp"

#include <initializer_list>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace GUI {

class Container;

using WidgetList = std::vector<std::shared_ptr<Widget>>;

class Layout {
public:
    Layout(Container& c)
        : m_container(c) { }

    virtual ~Layout() = default;

    virtual void run() = 0;

    void set_multipliers(std::initializer_list<float> list);

    // Padding = a gap between content (child widgets) and edge
    // TODO: Support separate paddings for sides
    void set_padding(float m) { m_padding = m; }
    float padding() const { return m_padding; }

    std::vector<float> m_multipliers;

protected:
    Container& m_container;
    float m_padding = 0;

    WidgetList& widgets();
};

enum class Orientation {
    Horizontal,
    Vertical
};

/// Widgets are resized to fill up the entire space (in the vertical axis)
class BoxLayout : public Layout {
public:
    BoxLayout(Container& c, Orientation o)
        : Layout(c)
        , m_orientation(o) { }

    // Spacing = a gap between widgets (but not between edges and widgets)
    void set_spacing(float s) { m_spacing = s; }
    virtual void run() override;

    enum class ContentAlignment {
        BoxStart,
        BoxEnd
    };

    void set_content_alignment(ContentAlignment alignment) { m_content_alignment = alignment; }

private:
    Orientation m_orientation;
    ContentAlignment m_content_alignment = ContentAlignment::BoxStart;
    float m_spacing = 0;
};

class VerticalBoxLayout : public BoxLayout {
public:
    VerticalBoxLayout(Container& c)
        : BoxLayout(c, Orientation::Vertical) { }
};

class HorizontalBoxLayout : public BoxLayout {
public:
    HorizontalBoxLayout(Container& c)
        : BoxLayout(c, Orientation::Horizontal) { }
};

// Just assigns input_size to size.
class BasicLayout : public Layout {
public:
    BasicLayout(Container& c)
        : Layout(c) { }

private:
    virtual void run() override;
};

class Container : public Widget {
public:
    explicit Container(Container& parent)
        : Widget(parent) { }
        
    explicit Container(WidgetTreeRoot& parent)
        : Widget(parent) { }

    template<class T, class... Args>
    requires(std::is_base_of_v<Widget, T>&& requires(Container& c, Args&&... args) { T(c, args...); })
        T* add_widget(Args&&... args) {
        auto widget = std::make_shared<T>(*this, std::forward<Args>(args)...);
        m_widgets.push_back(widget);
        if (m_layout)
            m_layout->m_multipliers.push_back(1);
        set_needs_relayout();
        return widget.get();
    }

    void add_created_widget(std::shared_ptr<Widget> widget) {
        m_widgets.push_back(std::move(widget));
        if (m_layout)
            m_layout->m_multipliers.push_back(1);
        set_needs_relayout();
    }

    virtual void do_update() override;
    virtual void do_handle_event(Event&) override;
    virtual void do_relayout() override;
    virtual void do_draw(sf::RenderWindow& window) const override;

    template<class T, class... Args>
    requires(std::is_base_of_v<Layout, T>&& requires(Container& c, Args&&... args) { T(c, args...); })
        T& set_layout(Args&&... args) {
        auto layout = std::make_unique<T>(*this, std::forward<Args>(args)...);
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

protected:

    virtual void relayout() override;
    virtual void handle_event(Event&) override;
    virtual float intrinsic_padding() const { return 0; }
    virtual void focus_first_child_or_self() override;
    virtual bool accepts_focus() const override;

    // Isolated focus - so that the widget cannot be focused from outside
    // and the focus cannot "escape" the widget using Tab. Used for settings
    // windows and settings menu so that you can "circulate" all settings
    // using Tab.
    // FIXME: Allow user to set it for any container.
    virtual bool isolated_focus() const { return false; }

    std::optional<size_t> focused_widget_index(bool recursive) const;
    // Returns true if the focus changed (one of children was focused)
    bool focus_next_widget(bool called_from_child);
    void focus_first_child();

    WidgetList m_widgets;

private:
    friend Layout;
    void m_find_widgets_by_class_name_recursively_helper(std::string_view class_name, std::vector<Widget*>& vec) const;

    std::unique_ptr<Layout> m_layout;
};

}
