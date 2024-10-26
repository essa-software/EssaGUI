#include "Essa/GUI/EML/Formatter.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/SyntaxHighlighter.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/MDI/Host.hpp>
#include <Essa/GUI/Widgets/Splitter.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <Essa/GUI/WindowRoot.hpp>
#include <EssaUtil/Error.hpp>
#include <EssaUtil/GenericParser.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 1280, 720 }, "EML Playground");

    auto& container = host_window.set_root_widget<GUI::Container>();
    container.set_layout<GUI::VerticalBoxLayout>();

    auto* toolbar = container.add_widget<GUI::Container>();
    toolbar->set_layout<GUI::HorizontalBoxLayout>();
    toolbar->set_size({ Util::Length::Auto, 32.0_px });

    auto* open_window = toolbar->add_widget<GUI::TextButton>();
    open_window->set_content("Open Window");
    // open_window->set_size({ 120.0_px, Util::Length::Auto });

    auto* format = toolbar->add_widget<GUI::TextButton>();
    format->set_content("Format");
    // format->set_size({ 120.0_px, Util::Length::Auto });

    auto* preview = container.add_widget<GUI::HorizontalSplitter>();

    auto* eml_editor = preview->add_widget<GUI::TextEditor>();
    auto* eml_container = preview->add_widget<GUI::MDI::Host>();

    GUI::WindowRoot* tool_window = nullptr;
    eml_editor->on_change = [&](Util::UString const& text) {
        auto& container = eml_container->set_background_widget<GUI::Container>();
        (void)container.load_from_eml_string(text.encode());
        if (tool_window) {
            (void)tool_window->load_from_eml_string(text.encode());
        }
    };
    eml_editor->set_syntax_highlighter(std::make_unique<EML::SyntaxHighlighter>());

    eml_editor->set_size({ 80_perc, Util::Length::Initial });

    open_window->on_click = [&]() {
        if (tool_window) {
            tool_window->window().close();
        }
        auto opened_overlay = eml_container->open_window();
        (void)opened_overlay.root.load_from_eml_string(eml_editor->content().encode());
        tool_window = &opened_overlay.root;
        opened_overlay.window.on_close = [&tool_window, new_window_root = &opened_overlay.root]() {
            if (tool_window == new_window_root) {
                tool_window = nullptr;
            }
        };
    };

    format->on_click = [&]() {
        auto formatted = EML::Formatter { eml_editor->content().encode() }.format();
        eml_editor->set_content(Util::UString(formatted));
    };

    app.run();
    return 0;
}
