#pragma once

#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/MDI/Window.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/GUI/WindowRoot.hpp>

namespace GUI::MDI {

class Overlay;
class Window;

class Host : public GUI::Widget {
public:
    Host();

    template<class T, class... Args>
        requires std::is_base_of_v<GUI::Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_background_widget(Args&&... args) {
        return m_background_overlay->set_main_widget<T>(std::forward<Args>(args)...);
    }

    template<class T> auto& set_created_background_widget(std::shared_ptr<T> w) {
        return m_background_overlay->set_created_main_widget<T>(std::move(w));
    }

    auto* background_widget() { return m_background_overlay->main_widget(); }

    template<class T, class... Args>
        requires(std::is_base_of_v<Overlay, T>)
    /*deprecated*/ T& open_overlay(Args&&... args) {
        auto& overlay = static_cast<T&>(open_overlay_impl(std::make_unique<T>(*this, std::forward<Args>(args)...)));
        // Root is already created by the dirty hack in Overlay constructor
        return overlay;
    }

    template<class T>
        requires(std::is_base_of_v<WindowRoot, T>)
    struct OpenedOverlay {
        Overlay& overlay;
        T& window_root;
    };

    template<class T = WindowRoot, class... Args>
        requires(std::is_base_of_v<WindowRoot, T>)
    OpenedOverlay<T> open_overlay(Args&&... args) {
        auto& overlay = open_overlay_impl(std::make_unique<Window>(*this, "TODO: OverlayID"));
        auto wnd_root = std::make_unique<T>(overlay, std::forward<Args>(args)...);
        auto wnd_root_ptr = wnd_root.get();
        overlay.set_root(std::move(wnd_root));
        return {
            .overlay = overlay,
            .window_root = *wnd_root_ptr,
        };
    }

    // FIXME: Generalize it like normal open_overlay
    struct OpenOrFocusResult {
        Window* window {};
        bool opened {};
    };
    OpenOrFocusResult open_or_focus_window(Util::UString const& title, std::string id);

    Overlay* focused_overlay() const { return m_focused_overlay; }
    void focus_overlay(Overlay&);

    template<class Callback> void for_each_overlay(Callback&& callback) {
        for (auto& wnd : m_overlays)
            callback(*wnd);
    }

    virtual EventHandlerResult do_handle_event(GUI::Event const&) override;

    /*restricted(HostWindow)*/ void remove_closed_overlays();

private:
    using OverlayList = std::list<std::unique_ptr<Overlay>>;

    virtual void draw(Gfx::Painter& painter) const override;
    virtual void update() override;
    virtual void relayout() override;
    virtual bool accepts_focus() const override { return true; }
    virtual bool steals_focus() const override { return true; }

    Overlay& open_overlay_impl(std::unique_ptr<Overlay>);
    void focus_window(OverlayList::iterator);

    Overlay* m_background_overlay = nullptr;
    OverlayList m_overlays;
    Util::Point2u m_next_overlay_position;
    Overlay* m_focused_overlay = nullptr;
    Overlay* m_hovered_overlay = nullptr;
};

}
