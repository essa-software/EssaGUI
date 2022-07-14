#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Console.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Theme.hpp>
#include <EssaGUI/gui/ToolWindow.hpp>

int main() {
    GUI::Theme theme;
    theme.load_ini("../examples/test_ini.ini");
    return 0;
}
