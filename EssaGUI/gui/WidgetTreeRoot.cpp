#include "WidgetTreeRoot.hpp"

#include "Application.hpp"
#include "Widget.hpp"

#include <EssaGUI/eml/Loader.hpp>
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <cassert>
#include <iostream>

namespace GUI {

void WidgetTreeRoot::draw(Window& window) {
    window.set_projection(llgl::Projection::ortho({ Util::Rectd { Util::Vector2d {}, Util::Vector2d { window.size() } } }, window.rect()));

    if (!m_main_widget)
        return;
    if (m_needs_relayout) {
        // std::cout << m_id << "\n"
        m_main_widget->set_size({ { size().x(), Util::Length::Px }, { size().y(), Util::Length::Px } });
        m_main_widget->set_raw_size(size());
        m_main_widget->do_relayout();
        m_main_widget->dump(0);
        m_needs_relayout = false;
    }
    m_main_widget->do_draw(window);
}

void WidgetTreeRoot::handle_event(llgl::Event event) {
    if (!m_main_widget)
        return;
    Event gui_event(event);
    m_main_widget->do_handle_event(gui_event);
}

void WidgetTreeRoot::handle_events() {
    // This event handler just takes all the events
    // (except global events) and passes the to the
    // underlying main_widget. This is used for modal
    // windows.
    // FIXME: Support moving other ToolWindows even
    //        if other modal window is open.
    auto& app = GUI::Application::the();
    for (auto& host_window : app.host_windows()) {
        llgl::Event event;
        while (host_window.window().poll_event(event)) {
            handle_event(transform_event(position(), event));
            if (event.type == llgl::Event::Type::Resize)
                host_window.handle_event(event);
        }
    }
}

void WidgetTreeRoot::tick() {
    // Handle events only for the current WTR...
    // (so that e.g. the simulation doesn't run)
    handle_events();
    update();

    // HACK: Check for Closed events on App and break
    //       this loop so that we can quit safely.
    if (!Application::the().is_running())
        quit();

    // ...but redraw the entire Application!
    Application::the().redraw_all_host_windows();

    // Remove closed overlays on Application so
    // that closed windows actually are closed
    if (!is_running())
        return;
    for (auto& host_window : Application::the().host_windows())
        host_window.remove_closed_overlays();
}

llgl::Event WidgetTreeRoot::transform_event(Util::Vector2f offset, llgl::Event event) const {
    if (event.type == llgl::Event::Type::MouseButtonPress || event.type == llgl::Event::Type::MouseButtonRelease) {
        event.mouse_button.position.x -= offset.x();
        event.mouse_button.position.y -= offset.y();
    }
    else if (event.type == llgl::Event::Type::MouseMove) {
        event.mouse_move.position.x -= offset.x();
        event.mouse_move.position.y -= offset.y();
    }

    return event;
}

Theme const& WidgetTreeRoot::theme() const {
    return Application::the().theme();
}

Gfx::ResourceManager const& WidgetTreeRoot::resource_manager() const {
    return Application::the().resource_manager();
}

EML::EMLErrorOr<void> WidgetTreeRoot::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    m_main_widget = TRY(object.require_and_construct_object<Widget>("main_widget", loader, *this));
    return {};
}

}
