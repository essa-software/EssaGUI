#pragma once

#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

namespace GUI {

class WidgetTreeRoot;

// WindowRoot is a class for storing window-local user data
// that can be loaded from EML. This is to support attaching
// them to any kind of Window (HostWindow, MDIWindow).
// FIXME: Some better names (GUIScreen?)
class WindowRoot : public EML::EMLObject {
public:
    explicit WindowRoot(WidgetTreeRoot& wnd)
        : m_window(wnd) { }

    auto& window() { return m_window; }

    CREATE_VALUE(Widget*, focused_widget, nullptr)

    void set_needs_relayout() { m_needs_relayout = true; }

    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_main_widget(Args&&... args) {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        auto widget_ptr = widget.get();
        m_main_widget = std::move(widget);
        m_main_widget->set_window_root(*this);
        m_main_widget->init();
        m_needs_relayout = true;
        return *widget_ptr;
    }

    template<class T, class... Args> auto& set_created_main_widget(std::unique_ptr<T> w) {
        auto widget_ptr = w.get();
        m_main_widget = std::move(w);
        m_main_widget->set_window_root(*this);
        m_main_widget->init();
        m_needs_relayout = true;
        return *widget_ptr;
    }

    void update() {
        if (m_main_widget)
            m_main_widget->do_update();
    }

    auto* main_widget() { return m_main_widget.get(); }

    /*restricted(WidgetTreeRoot)*/ void relayout_and_draw(Gfx::Painter&);
    /*restricted(WidgetTreeRoot)*/ void do_handle_event(GUI::Event const& event);

protected:
    void close();

private:
    virtual Widget::EventHandlerResult handle_event(GUI::Event const&) { return Widget::EventHandlerResult::NotAccepted; }
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader&) override;

    bool m_needs_relayout = true;
    std::unique_ptr<Widget> m_main_widget;
    WidgetTreeRoot& m_window;
};

}
