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

namespace GUI {

HostWindow::HostWindow(Util::Size2u size, Util::UString const& title, llgl::WindowSettings const& settings)
    : llgl::Window(size, title, settings) {
    // FIXME: Support custom window roots (that's the point of them after all!)
    set_root(std::make_unique<WindowRoot>(*this));
    set_root_widget<MDI::Host>();
    m_legacy_mdi_host = static_cast<MDI::Host*>(root_widget());
    m_legacy_mdi_host->set_raw_size(size.cast<int>());
    set_active();
    llgl::opengl::enable_debug_output();
    m_painter.construct(renderer());
}

void HostWindow::setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const& settings) {
    create(size, title, settings);
    m_painter.construct(renderer());
}

void HostWindow::close() { llgl::Window::close(); }

void HostWindow::center_on_screen() { llgl::Window::center_on_screen(); }

DBG_DECLARE(GUI_DumpOverlayHandleEventCalls);

void HostWindow::handle_events() {
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
    // hacky hacky hacky hacky
    set_active();
    renderer().clear(m_background_color);
    OpenGL::Clear(GL_DEPTH_BUFFER_BIT);
    m_painter->reset();

    Util::Recti viewport { {}, size() };
    m_painter->builder().set_projection(llgl::Projection::ortho({ Util::Rectd { {}, size().cast<double>() } }, Util::Recti { viewport }));

    WidgetTreeRoot::draw(*m_painter);

    m_painter->render();
    display();
}

void HostWindow::open_context_menu(ContextMenu menu, Util::Point2i position) {
    auto menu_overlay = GUI::Application::the().open_host_window<ContextMenuOverlay>(std::move(menu));
    menu_overlay.window.set_position(position);
    menu_overlay.window.show_modal();
}

TooltipOverlay& HostWindow::add_tooltip(Tooltip t) {
    // TODO: Use native (Host) windows
    auto& overlay = m_legacy_mdi_host->open_overlay<TooltipOverlay>(std::move(t));
    auto& container = overlay.set_main_widget<Container>();
    container.set_layout<HorizontalBoxLayout>();
    return overlay;
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
