#include "Essa/GUI/Overlays/MessageBox.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/MDI/MDIWidget.hpp>

int main() {
    GUI::SimpleApplication<GUI::MDIWidget> app("Test");

    auto& widget = app.main_widget();
    widget.open_overlay<GUI::MessageBox>();

    app.run();

    return 0;
}
