#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Theme.hpp>
#include <Essa/GUI/Widgets/Console.hpp>
#include <Essa/GUI/Widgets/Container.hpp>

int main() {
    GUI::Theme theme;
    auto error = theme.load_ini("../examples/test_ini.ini");
    if (error.is_error()) {
        std::cout << "Error loading theme: " << error.error().function << ": " << strerror(error.error().error) << std::endl;
        return 1;
    }
    return 0;
}
