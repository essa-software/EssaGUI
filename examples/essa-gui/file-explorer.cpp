#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/FileExplorer.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 1000, 1000 }, "File Explorer");

    auto& file_explorer = host_window.open_overlay<GUI::FileExplorer>();
    file_explorer.set_size({ 1000, 500 });
    file_explorer.center_on_screen();
    file_explorer.open_path("/");
    file_explorer.run();

    app.run();
    return 0;
}
