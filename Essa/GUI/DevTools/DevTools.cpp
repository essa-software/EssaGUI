#include "DevTools.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/DevTools/Object.hpp>
#include <Essa/GUI/Widgets/Checkbox.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/ScrollableContainer.hpp>
#include <Essa/GUI/Widgets/TreeView.hpp>
#include <EssaUtil/Config.hpp>

namespace GUI {

class LayoutTreeModel : public GUI::Model {
public:
    static inline const ModelColumn COLUMNS[] = {
        { .width = 300, .name = "Name" },
    };

    virtual size_t column_count() const override { return std::size(COLUMNS); }
    virtual ModelColumn column(size_t column) const override { return COLUMNS[column]; }
    virtual GUI::Variant data(Node node, [[maybe_unused]] size_t column) const override {
        auto const& dto = *static_cast<GUI::DevToolsObject const*>(node.data.data);
        auto const* type = typeid(dto).name();
        auto name = dto.dev_tools_name();
        if (name.is_empty()) {
            return Util::UString(type);
        }
        return Util::UString::format("{}: {}", type, name.encode());
    }
    // virtual llgl::Texture const* icon(Node) const { return nullptr; }
    virtual size_t children_count(std::optional<Node> parent) const override {
        if (!parent) {
            return GUI::Application::the().dev_tools_children_count();
        }
        return static_cast<DevToolsObject const*>(parent->data.data)->dev_tools_children_count();
    }
    virtual NodeData child(std::optional<Node> parent, size_t idx) const override {
        if (!parent) {
            return NodeData { .type = 0, .data = GUI::Application::the().dev_tools_children()[idx] };
        }
        return NodeData { .type = 0, .data = static_cast<DevToolsObject const*>(parent->data.data)->dev_tools_children()[idx] };
    }
};

DevTools::DevTools(WidgetTreeRoot& wnd)
    : WindowRoot(wnd) {
    (void)load_from_eml_resource(GUI::Application::the().resource_manager().require<EML::EMLResource>("DevTools.eml"));

    auto& main_widget = *static_cast<GUI::Container*>(this->main_widget());
    auto& switches_scroll = main_widget.find<GUI::ScrollableContainer>("switches_scroll");
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

    auto& layout_treeview = main_widget.find<GUI::TreeView>("layout_tree");
    layout_treeview.set_model(std::make_unique<LayoutTreeModel>());
}

}
