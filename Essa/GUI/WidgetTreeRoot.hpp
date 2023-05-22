#pragma once

#include "EssaUtil/Rect.hpp"
#include "EventLoop.hpp"
#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <list>
#include <memory>

namespace GUI {

class WidgetTreeRoot
    : public EventLoop
    , public EML::EMLObject {
public:
    WidgetTreeRoot() = default;
    WidgetTreeRoot(WidgetTreeRoot const&) = delete;
    WidgetTreeRoot& operator=(WidgetTreeRoot const&) = delete;

    virtual ~WidgetTreeRoot() = default;

    CREATE_VALUE(Widget*, focused_widget, nullptr)

    void set_needs_relayout() { m_needs_relayout = true; }

    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_main_widget(Args&&... args) {
        auto widget = std::make_shared<T>(std::forward<Args>(args)...);
        auto widget_ptr = widget.get();
        m_main_widget = std::move(widget);
        m_main_widget->set_widget_tree_root(*this);
        m_main_widget->init();
        m_needs_relayout = true;
        return *widget_ptr;
    }

    template<class T, class... Args> auto& set_created_main_widget(std::shared_ptr<T> w) {
        auto widget_ptr = w.get();
        m_main_widget = std::move(w);
        m_main_widget->set_widget_tree_root(*this);
        m_main_widget->init();
        m_needs_relayout = true;
        return *widget_ptr;
    }

    auto* main_widget() { return m_main_widget.get(); }

    CREATE_VALUE(std::string, id, "")

    virtual void draw(Gfx::Painter&);
    virtual void handle_event(GUI::Event const&);
    virtual void handle_events();
    virtual void update() {
        if (m_main_widget)
            m_main_widget->do_update();
    }

    virtual Util::Point2i position() const = 0;
    virtual Util::Size2i size() const = 0;
    Util::Recti rect() const { return { position(), size() }; }

    // The rect that the WidgetTreeRoot should consume events from. For
    // ToolWindows, it is content + titlebar + resize rect.
    virtual Util::Recti full_rect() const { return rect(); }

    void show_modal() {
        m_modal = true;
        run();
    }

    bool is_modal() const { return m_modal; }

protected:
    using EventLoop::run;

    virtual void tick() override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

private:
    bool m_needs_relayout = true;
    std::shared_ptr<Widget> m_main_widget;
    bool m_modal = false;
};

}
