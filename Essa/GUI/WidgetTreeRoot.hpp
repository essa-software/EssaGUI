#pragma once

#include "EventLoop.hpp"
#include <Essa/GUI/EML/EMLObject.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/GUI/WindowRoot.hpp>
#include <EssaUtil/Rect.hpp>
#include <list>
#include <memory>

namespace GUI {

// TODO: Rename this to AbstractWindow or something like this
class WidgetTreeRoot
    : public EventLoop
    , public EML::EMLObject {
public:
    WidgetTreeRoot() = default;
    WidgetTreeRoot(WidgetTreeRoot const&) = delete;
    WidgetTreeRoot& operator=(WidgetTreeRoot const&) = delete;

    virtual ~WidgetTreeRoot() = default;

    CREATE_VALUE(std::string, id, "")

    virtual void setup(Util::UString title, Util::Size2u size) = 0;
    virtual void center_on_screen() = 0;
    virtual void close() = 0;

    virtual void draw(Gfx::Painter&);
    virtual void handle_event(GUI::Event const&);
    void handle_events();

    virtual void update() { m_root->update(); }

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

    template<class T, class... Args>
        requires std::is_base_of_v<Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_main_widget(Args&&... args) {
        return m_root->set_main_widget<T>(std::forward<Args>(args)...);
    }
    template<class T> auto& set_created_main_widget(std::unique_ptr<T> w) { return m_root->set_created_main_widget<T>(std::move(w)); }

    auto* main_widget() { return m_root->main_widget(); }

    WindowRoot const& root() const { return *m_root; }
    WindowRoot& root() { return *m_root; }

protected:
    void set_root(std::unique_ptr<WindowRoot> root) { m_root = std::move(root); }

    using EventLoop::run;

    virtual void tick() override;
    // Deprecated. Override WindowRoot::load_from_eml_object for window-agnostic EML loaders.
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    Theme const& theme() const;
    Gfx::ResourceManager const& resource_manager() const;

private:
    std::unique_ptr<WindowRoot> m_root;
    bool m_modal = false;
};

}
