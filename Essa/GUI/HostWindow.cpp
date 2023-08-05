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
}

void HostWindow::setup(Util::UString title, Util::Size2u size, llgl::WindowSettings const& settings) { create(size, title, settings); }

void HostWindow::close() { llgl::Window::close(); }

void HostWindow::center_on_screen() { llgl::Window::center_on_screen(); }

DBG_DECLARE(GUI_DumpOverlayHandleEventCalls);

void HostWindow::handle_events() {
    while (true) {
        if (is_closed()) {
            return;
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
    m_painter.reset();

    Util::Recti viewport { {}, size() };
    m_painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { {}, size().cast<double>() } }, Util::Recti { viewport }));

    WidgetTreeRoot::draw(m_painter);

    m_painter.render();
    display();
}

void HostWindow::open_context_menu(ContextMenu context_menu, Util::Point2i position) {
    // TODO: Use native (Host) windows
    auto& menu = m_legacy_mdi_host->open_overlay<ContextMenuOverlay>(context_menu, position);
    menu.show_modal();
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

}
