#include "DevTools.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/Widgets/Checkbox.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/ScrollableContainer.hpp>

namespace GUI {

DevTools::DevTools(WidgetTreeRoot& wnd)
    : WindowRoot(wnd) {
    (void)load_from_eml_resource(GUI::Application::the().resource_manager().require<EML::EMLResource>("DevTools.eml"));

    auto& switches_scroll = static_cast<GUI::Container*>(this->main_widget())->find<GUI::ScrollableContainer>("switches_scroll");
    auto& switches_container = static_cast<GUI::Container&>(*switches_scroll.widget());

    for (auto const& switch_ : DebugSwitch::switches()) {
        if (switch_.first == "DevTools") {
            // Disable this footgun.
            continue;
        }
        if (switch_.first == "DumpSwitches") {
            // Useless to change, since this works only on startup
            continue;
        }
        auto& checkbox = *switches_container.add_widget<GUI::Checkbox>();
        checkbox.set_caption(Util::UString(switch_.first));
        checkbox.set_active(switch_.second->get());
        checkbox.on_change = [switch_ = switch_.second](bool v) { switch_->set(v); };
    }
}

}
