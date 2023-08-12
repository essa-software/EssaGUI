#include "WidgetTreeRoot.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/ClipViewScope.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <cassert>
#include <iostream>

namespace GUI {

void WidgetTreeRoot::draw(Gfx::Painter& painter) { m_root->relayout_and_draw(painter); }
void WidgetTreeRoot::handle_event(llgl::Event const& event) {
    if (on_event && on_event(event) == Widget::EventHandlerResult::Accepted) {
        return;
    }
    m_root->do_handle_event(event);
}

Theme const& WidgetTreeRoot::theme() const { return Application::the().theme(); }

Gfx::ResourceManager const& WidgetTreeRoot::resource_manager() const { return Application::the().resource_manager(); }

EML::EMLErrorOr<void> WidgetTreeRoot::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    m_root->set_created_main_widget(TRY(object.require_and_construct_object<Widget>("main_widget", loader, *m_root)));
    return {};
}

}
