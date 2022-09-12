#include "WidgetTreeRoot.hpp"

#include "Application.hpp"
#include "Widget.hpp"

#include <EssaGUI/eml/Loader.hpp>
#include <EssaGUI/gfx/ClipViewScope.hpp>
#include <cassert>
#include <iostream>

namespace GUI {

void WidgetTreeRoot::draw() {
    m_window.set_projection(llgl::Projection::ortho({ Util::Rectd { Util::Vector2d {}, Util::Vector2d { window().size() } } }, window().rect()));

    if (!m_main_widget)
        return;
    if (m_needs_relayout) {
        // std::cout << m_id << "\n"
        m_main_widget->set_size({ { size().x(), Length::Unit::Px }, { size().y(), Length::Unit::Px } });
        m_main_widget->set_raw_size(size());
        m_main_widget->do_relayout();
        m_main_widget->dump(0);
        m_needs_relayout = false;
    }
    m_main_widget->do_draw(m_window);
}

void WidgetTreeRoot::handle_event(llgl::Event event) {
    if (!m_main_widget)
        return;
    Event gui_event(event);
    m_main_widget->do_handle_event(gui_event);
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
    window().clear();
    Application::the().draw();
    window().display();

    // Remove closed overlays on Application so
    // that closed windows actually are closed
    if (!is_running())
        return;
    Application::the().remove_closed_overlays();
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
