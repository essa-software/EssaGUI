#pragma once

#include "EventLoop.hpp"
#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/GUI/WindowRoot.hpp>
#include <Essa/LLGL/Window/WindowSettings.hpp>
#include <EssaUtil/Rect.hpp>
#include <list>
#include <memory>

namespace GUI {

// TODO: Rename this to AbstractWindow or something like this
class WidgetTreeRoot
    : public EML::EMLObject
    , public DevToolsObject {
public:
    WidgetTreeRoot() = default;
    WidgetTreeRoot(WidgetTreeRoot const&) = delete;
    WidgetTreeRoot& operator=(WidgetTreeRoot const&) = delete;

    virtual ~WidgetTreeRoot() = default;

    CREATE_VALUE(std::string, id, "")

    virtual void setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const&) = 0;
    virtual void set_size(Util::Size2i) = 0;
    virtual void center_on_screen() = 0;
    virtual void close() = 0;
    virtual HostWindow& host_window() = 0;

    virtual void draw(Gfx::Painter&);
    virtual void handle_event(llgl::Event const&);
    void handle_events();

    virtual void update() { m_root->update(); }

    // Position relative to HostWindow. For HostWindows itself, it's 0.
    // Used in Widget::host_position(), and to calculate widget-relative
    // mouse position from llgl::mouse_position().
    virtual Util::Point2i host_position() const = 0;
    virtual Util::Size2i size() const = 0;

    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_main_widget(Args&&... args) {
        return m_root->set_main_widget<T>(std::forward<Args>(args)...);
    }
    template<class T> auto& set_created_main_widget(std::unique_ptr<T> w) { return m_root->set_created_main_widget<T>(std::move(w)); }

    auto* main_widget() { return m_root->main_widget(); }

    WindowRoot const& root() const { return *m_root; }
    WindowRoot& root() { return *m_root; }

    // Override default event handler. If this returns Accepted,
    // no events will be passed to widgets.
    std::function<GUI::Widget::EventHandlerResult(llgl::Event const&)> on_event;

    virtual std::vector<DevToolsObject const*> dev_tools_children() const override;
    virtual Util::UString dev_tools_name() const override { return "Generic WTR"; }

protected:
    void set_root(std::unique_ptr<WindowRoot> root) { m_root = std::move(root); }

    // Deprecated. Override WindowRoot::load_from_eml_object for window-agnostic EML loaders.
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

private:
    std::unique_ptr<WindowRoot> m_root;
};

}
