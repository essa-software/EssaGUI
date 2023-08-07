#pragma once

#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/MDI/Window.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/GUI/WindowRoot.hpp>

namespace GUI::MDI {

class Window;

class Host : public GUI::Widget {
public:
    Host();

    template<class T, class... Args>
        requires std::is_base_of_v<GUI::Widget, T> && std::is_constructible_v<T, Args...>
    auto& set_background_widget(Args&&... args) {
        return m_background_window->set_main_widget<T>(std::forward<Args>(args)...);
    }

    template<class T> auto& set_created_background_widget(std::shared_ptr<T> w) {
        return m_background_window->set_created_main_widget<T>(std::move(w));
    }

    auto* background_widget() { return m_background_window->main_widget(); }

    template<class T>
        requires(std::is_base_of_v<WindowRoot, T>)
    struct OpenedWindow {
        Window& window;
        T& root;
    };

    template<class T = WindowRoot, class... Args>
        requires(std::is_base_of_v<WindowRoot, T>)
    OpenedWindow<T> open_window(Args&&... args) {
        auto& overlay = open_window_impl(std::make_unique<Window>(*this));
        auto wnd_root = std::make_unique<T>(overlay, std::forward<Args>(args)...);
        auto wnd_root_ptr = wnd_root.get();
        overlay.set_root(std::move(wnd_root));
        return {
            .window = overlay,
            .root = *wnd_root_ptr,
        };
    }

    // FIXME: Generalize it like normal open_overlay
    struct OpenOrFocusResult {
        Window* window {};
        bool opened {};
    };
    OpenOrFocusResult open_or_focus_window(Util::UString const& title, std::string id);

    Window* focused_window() const { return m_focused_window; }
    void focus_window(Window&);

    template<class Callback> void for_each_window(Callback&& callback) {
        for (auto& wnd : m_windows)
            callback(*wnd);
    }

    virtual EventHandlerResult do_handle_event(GUI::Event const&) override;

    /*restricted(HostWindow)*/ void remove_closed_windows();

private:
    using WindowList = std::list<std::unique_ptr<Window>>;

    virtual void draw(Gfx::Painter& painter) const override;
    virtual void update() override;
    virtual void relayout() override;
    virtual bool accepts_focus() const override { return true; }
    virtual bool steals_focus() const override { return true; }

    Window& open_window_impl(std::unique_ptr<Window>);
    void focus_window_it(WindowList::iterator);

    Window* m_background_window = nullptr;
    WindowList m_windows;
    Util::Point2u m_next_window_position;
    Window* m_focused_window = nullptr;
    Window* m_hovered_window = nullptr;
};

}
