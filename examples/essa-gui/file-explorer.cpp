#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>

int main() {
    GUI::Application app;

    auto path = GUI::FileExplorer::get_path_to_open();
    fmt::print("path: {}\n", path ? path->string() : "None");

    app.run();
    return 0;
}
