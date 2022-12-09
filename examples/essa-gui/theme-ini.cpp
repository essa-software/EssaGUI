#include <EssaGUI/Application.hpp>
#include <EssaGUI/Widgets/Console.hpp>
#include <EssaGUI/Widgets/Container.hpp>
#include <EssaGUI/Theme.hpp>
#include <EssaGUI/Overlays/ToolWindow.hpp>

int main() {
    GUI::Theme theme;
    auto error = theme.load_ini("../examples/test_ini.ini");
    if (error.is_error()) {
        std::cout << "Error loading theme: " << error.error().function << ": " << strerror(error.error().error) << std::endl;
        return 1;
    }
    return 0;
}
