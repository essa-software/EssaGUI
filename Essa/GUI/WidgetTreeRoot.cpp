#include "WidgetTreeRoot.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <cassert>
#include <iostream>

namespace GUI {

void WidgetTreeRoot::draw(Gfx::Painter& painter) { m_root->relayout_and_draw(painter); }
void WidgetTreeRoot::handle_event(GUI::Event const& event) { m_root->do_handle_event(event); }

void WidgetTreeRoot::handle_events() {
    // This event handler just takes all the events
    // (except global events) and passes the to the
    // underlying main_widget. This is used for modal
    // windows.
    // FIXME: Support moving other ToolWindows even
    //        if other modal window is open.
    auto& app = GUI::Application::the();
    for (auto& host_window : app.host_windows()) {
        while (true) {
            auto event = host_window.poll_event();
            if (!event) {
                break;
            }
            handle_event(event->relativized(position().to_vector()));

            // We need to inform host window about global events, as now we act as
            // main event loop!
            if (event->target_type() == llgl::EventTargetType::Global) {
                host_window.handle_event(*event);
            }
        }
    }
}

void WidgetTreeRoot::tick() {
    // Handle events only for the current WTR...
    // (so that e.g. the simulation doesn't run)
    handle_events();
    update();

    // Check for Closed events on App and break
    // this loop so that we can quit safely.
    if (!Application::the().is_running())
        quit();

    // ...but redraw the entire Application!
    Application::the().redraw_all_host_windows();

    // HACK: Remove closed overlays on Application so
    // that closed windows actually are closed
    if (!is_running())
        return;
    for (auto& host_window : Application::the().host_windows())
        host_window.remove_closed_overlays();
}

Theme const& WidgetTreeRoot::theme() const { return Application::the().theme(); }

Gfx::ResourceManager const& WidgetTreeRoot::resource_manager() const { return Application::the().resource_manager(); }

EML::EMLErrorOr<void> WidgetTreeRoot::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    m_root->set_created_main_widget(TRY(object.require_and_construct_object<Widget>("main_widget", loader, *m_root)));
    return {};
}

}
