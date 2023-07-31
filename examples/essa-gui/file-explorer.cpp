#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 1000, 1000 }, "File Explorer");

    auto file_explorer = host_window.open_overlay<GUI::FileExplorer>();
    file_explorer.window_root.open_path("/");
    file_explorer.overlay.show_modal();

    app.run();
    return 0;
}
