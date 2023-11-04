#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

class MainWidget : public GUI::Container {
public:
    virtual void on_init() {
        (void)load_from_eml_resource(resource_manager().require<EML::EMLResource>("MainWidget.eml")); //
    }
};

int main() {
    GUI::SimpleApplication<MainWidget> app { "Simple app", { 500, 500 } };
    app.run();
    return 0;
}
