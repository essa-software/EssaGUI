#pragma once

#include "EventLoop.hpp"
#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Graphics/Window.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <list>
#include <memory>

namespace GUI {

class WidgetTreeRoot : public EventLoop
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
        if (!m_main_widget->m_initialized) {
            m_main_widget->on_init();
            m_main_widget->m_initialized = true;
        }
        m_needs_relayout = true;
        return *widget_ptr;
    }

    template<class T, class... Args>
    auto& set_created_main_widget(std::shared_ptr<T> w) {
        auto widget_ptr = w.get();
        m_main_widget = std::move(w);
        m_main_widget->set_widget_tree_root(*this);
        if (!m_main_widget->m_initialized) {
            m_main_widget->on_init();
            m_main_widget->m_initialized = true;
        }
        m_needs_relayout = true;
        return *widget_ptr;
    }

    auto* main_widget() { return m_main_widget.get(); }

    CREATE_VALUE(std::string, id, "")

    virtual void draw(Gfx::Painter&);
    virtual void handle_event(llgl::Event);
    virtual void handle_events();
    virtual void update() {
        if (m_main_widget)
            m_main_widget->do_update();
    }

    virtual Util::Vector2f position() const = 0;
    virtual Util::Vector2f size() const = 0;
    Util::Rectf rect() const { return { position(), size() }; }

    // The rect that the WidgetTreeRoot should consume events from. For
    // ToolWindows, it is content + titlebar + resize rect.
    virtual Util::Rectf full_rect() const { return rect(); }

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

    llgl::Event transform_event(Util::Vector2f offset, llgl::Event event) const;

private:
    bool m_needs_relayout = true;
    std::shared_ptr<Widget> m_main_widget;
    bool m_modal = false;
};

}
