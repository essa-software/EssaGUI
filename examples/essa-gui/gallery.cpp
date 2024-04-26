#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Model.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Overlays/Prompt.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <EssaUtil/Config.hpp>

int main() {
    GUI::SimpleApplication<GUI::Container> app { "Widget Gallery", { 700, 500 } };
    (void)app.main_widget().load_from_eml_resource(app.resource_manager().require<EML::EMLResource>("Gallery.eml"));

    auto& file_explorer = app.main_widget().find<GUI::TextButton>("file_explorer");
    file_explorer.on_click = [&app]() {
        auto path = GUI::FileExplorer::get_path_to_open(&app.window());
        if (path) {
            GUI::message_box(
                &app.window(), Util::UString::format("Selected path: {}", path->string()), "Info",
                {
                    .buttons = GUI::MessageBox::Buttons::Ok,
                    .icon = GUI::MessageBox::Icon::Info,
                }
            );
        }
    };
    auto& prompt = app.main_widget().find<GUI::TextButton>("prompt");
    prompt.on_click = [&]() { GUI::prompt(app.window(), "Test prompt"); };

    auto& list = app.main_widget().find<GUI::TreeView>("list");

    class TestModel : public GUI::Model {
        virtual size_t column_count() const override {
            return 2;
        }
        virtual GUI::ModelColumn column(size_t column) const override {
            switch (column) {
            case 0:
                return GUI::ModelColumn { 100, "Row" };
            case 1:
                return GUI::ModelColumn { 100, "Is Child" };
            }
            ESSA_UNREACHABLE;
        }
        virtual GUI::Variant data(Node node, size_t column) const override {
            switch (column) {
            case 0:
                return Util::UString::format("{}", node.index);
            case 1:
                return Util::UString::format("{}", node.data.type == 1);
            }
            ESSA_UNREACHABLE;
        }
        virtual size_t children_count(std::optional<Node> parent) const override {
            return parent ? (parent->data.type == 0 ? 3 : 0) : 10;
        }
        virtual NodeData child(std::optional<Node> parent, size_t) const override {
            return parent ? NodeData { 1, nullptr } : NodeData { 0, nullptr };
        }
    };

    list.set_model(std::make_unique<TestModel>());

    app.run();
    return 0;
}
