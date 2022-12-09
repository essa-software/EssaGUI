#include <EssaGUI/GUI/Application.hpp>
#include <EssaGUI/GUI/Console.hpp>
#include <EssaGUI/GUI/Container.hpp>
#include <EssaGUI/GUI/Theme.hpp>
#include <EssaGUI/GUI/ToolWindow.hpp>

int main() {
    GUI::Theme theme;
    auto error = theme.load_ini("../examples/test_ini.ini");
    if (error.is_error()) {
        std::cout << "Error loading theme: " << error.error().function << ": " << strerror(error.error().error) << std::endl;
        return 1;
    }
    return 0;
}
