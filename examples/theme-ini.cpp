#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Console.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Theme.hpp>
#include <EssaGUI/gui/ToolWindow.hpp>

int main() {
    GUI::Theme theme;
    auto error = theme.load_ini("../examples/test_ini.ini");
    if (error.is_error()) {
        std::cout << "Error loading theme: " << error.error().function << ": " << strerror(error.error().error) << std::endl;
        return 1;
    }
    return 0;
}
