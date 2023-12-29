#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/MDI/Host.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <Essa/GUI/WindowRoot.hpp>

class MDIWindowRoot : public GUI::WindowRoot {
public:
    explicit MDIWindowRoot(GUI::WidgetTreeRoot& wtr)
        : GUI::WindowRoot(wtr) {
        (void)load_from_eml_resource(GUI::Application::the().resource_manager().require<EML::EMLResource>("MDIWindowRoot.eml"));
    }
};

int main() {
    GUI::Application app;
    auto& window = app.create_host_window({ 512, 512 }, "MDI");

    auto& mdi = window.set_main_widget<GUI::MDI::Host>();
    mdi.set_background_widget<GUI::TextEditor>();

    // FIXME: Here basically repeating what WindowRoot does on relayout. Maybe we should:
    // - add a helper function for that (force_relayout()??)
    // - even better, ensure that layout is done as soon as possible (E.g immediately after setting main widget)
    mdi.set_raw_size(window.size());
    mdi.do_relayout();

    for (size_t s = 0; s < 3; s++) {
        mdi.open_window<MDIWindowRoot>();
    }

    app.run();
    return 0;
}
