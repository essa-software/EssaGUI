#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Overlays/Prompt.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>

int main() {
    GUI::SimpleApplication<GUI::Container> app { "Widget Gallery", { 700, 500 } };
    (void)app.main_widget().load_from_eml_resource(app.resource_manager().require<EML::EMLResource>("Gallery.eml"));

    auto& file_explorer = app.main_widget().find<GUI::TextButton>("file_explorer");
    file_explorer.on_click = []() {
        auto path = GUI::FileExplorer::get_path_to_open();
        if (path) {
            GUI::message_box(
                Util::UString::format("Selected path: {}", path->string()), "Info",
                {
                    .buttons = GUI::MessageBox::Buttons::Ok,
                    .icon = GUI::MessageBox::Icon::Info,
                }
            );
        }
    };
    auto& prompt = app.main_widget().find<GUI::TextButton>("prompt");
    prompt.on_click = [&]() { GUI::prompt(app.window(), "Test prompt"); };

    app.run();
    return 0;
}
