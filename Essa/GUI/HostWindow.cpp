#include "HostWindow.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/LLGL/OpenGL/Error.hpp>
#include <Essa/LLGL/Window/Event.hpp>
#include <Essa/LLGL/Window/Window.hpp>

#include <cassert>
#include <tracy/Tracy.hpp>

namespace GUI {

HostWindow::HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& settings) {
    set_root(std::make_unique<WindowRoot>(*this));
    setup(title, size, settings);
}

void HostWindow::setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const& settings) {
    create(size, title, settings);
    set_active();
    llgl::opengl::enable_debug_output();
    m_painter.construct(renderer());
    set_background_color(GUI::Application::the().theme().window_background);
}

void HostWindow::close() { llgl::Window::close(); }

void HostWindow::center_on_screen() { llgl::Window::center_on_screen(); }

DBG_DECLARE(GUI_DumpOverlayHandleEventCalls);

void HostWindow::handle_events() {
    ZoneScoped;
    while (true) {
        if (is_closed()) {
            break;
        }
        auto event = poll_event();
        if (!event) {
            break;
        }
        handle_event(*event);
    }
}

void HostWindow::do_draw() {
    ZoneScopedN("HostWindow::draw");
    // hacky hacky hacky hacky
    set_active();
    renderer().clear(m_background_color);
    OpenGL::Clear(GL_DEPTH_BUFFER_BIT);
    m_painter->reset();

    Util::Recti viewport { {}, size() };
    m_painter->builder().set_projection(llgl::Projection::ortho({ Util::Rectd { {}, size().cast<double>() } }, Util::Recti { viewport }));

    WidgetTreeRoot::draw(*m_painter);

    m_painter->render();
    {
        ZoneScopedN("HostWindow display");
        display();
    }
}

void HostWindow::open_context_menu(ContextMenu menu, Util::Point2i position) {
    auto menu_overlay = GUI::Application::the().open_host_window<ContextMenuOverlay>(std::move(menu));
    menu_overlay.window.set_position(position);
    menu_overlay.window.show_modal();
}

TooltipOverlay& HostWindow::add_tooltip(Util::Point2u position, Tooltip t) const {
    Gfx::Text text(t.text, GUI::Application::the().font());
    auto tooltip_window = GUI::Application::the().open_host_window<TooltipOverlay>(std::move(t));
    tooltip_window.window.set_position(this->position() + position.cast<int>().to_vector());
    return tooltip_window.root;
}

Theme const& HostWindow::theme() const { return Application::the().theme(); }

Gfx::ResourceManager const& HostWindow::resource_manager() const { return Application::the().resource_manager(); }

void HostWindow::show_modal() {
    class ModalDialogEventLoop : public EventLoop {
    public:
        explicit ModalDialogEventLoop(HostWindow& window)
            : m_window(window) { }

    private:
        void handle_events() {
            // Handle all events for modal window
            m_window.handle_events();

            // Handle only Resize for other windows (for relayout)
            for (auto& window : GUI::Application::the().host_windows()) {
                if (&window == &m_window) {
                    continue;
                }
                while (true) {
                    if (window.is_closed()) {
                        break;
                    }
                    auto event = window.poll_event();
                    if (!event) {
                        break;
                    }
                    if (!event->is<llgl::EventTypes::WindowResizeEvent>()) {
                        continue;
                    }
                    window.handle_event(*event);
                }
            }
        }

        virtual void tick() override {
            handle_events();

            // Prevent use-after-free if modal window was closed in event handler.
            // This assumes that mutations can only happen there.
            if (m_window.is_closed()) {
                quit();
                return;
            }
            if (!is_running()) {
                return;
            }

            // Draw all windows
            GUI::Application::the().remove_closed_host_windows();
            GUI::Application::the().redraw_all_host_windows();
        }

        HostWindow& m_window;
    };

    ModalDialogEventLoop loop(*this);
    loop.run();
}

}
