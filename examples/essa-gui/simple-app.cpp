#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

int main() {
    GUI::SimpleApplication<GUI::Textfield> app { "Simple app", { 500, 500 } };
    auto& label = app.main_widget();
    label.set_content("test");
    label.set_alignment(GUI::Align::Center);
    app.run();
    return 0;
}
