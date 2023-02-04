#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Container.hpp>

int main() {
    GUI::SimpleApplication<GUI::Container> app { "Widget Gallery", { 700, 400 } };
    (void)app.main_widget().load_from_eml_resource(app.resource_manager().require<EML::EMLResource>("Gallery.eml"));
    app.run();
    return 0;
}
