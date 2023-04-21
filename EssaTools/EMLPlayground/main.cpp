#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/SyntaxHighlighter.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <EssaUtil/Error.hpp>
#include <EssaUtil/GenericParser.hpp>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "EML Playground");

    auto& container = host_window.set_main_widget<GUI::Container>();
    container.set_layout<GUI::VerticalBoxLayout>();

    auto toolbar = container.add_widget<GUI::Container>();
    toolbar->set_layout<GUI::HorizontalBoxLayout>();
    toolbar->set_size({ Util::Length::Auto, 32.0_px });

    auto* open_window = toolbar->add_widget<GUI::TextButton>();
    open_window->set_content("Open Window");
    open_window->set_size({ 120.0_px, Util::Length::Auto });

    auto* preview = container.add_widget<GUI::Container>();
    preview->set_layout<GUI::HorizontalBoxLayout>();

    auto eml_editor = preview->add_widget<GUI::TextEditor>();
    auto eml_container = preview->add_widget<GUI::Container>();

    GUI::ToolWindow* tool_window = nullptr;
    eml_editor->on_change = [&](Util::UString const& text) {
        eml_container->shrink(0);
        (void)eml_container->load_from_eml_string(text.encode());
        if (tool_window) {
            (void)tool_window->load_from_eml_string(text.encode());
            tool_window->set_position({ host_window.size().x() * 3 / 4 - tool_window->size().x() / 2, host_window.size().y() / 2 - tool_window->size().y() / 2 });
        }
    };
    eml_editor->set_syntax_highlighter(std::make_unique<EML::SyntaxHighlighter>());

    open_window->on_click = [&]() {
        tool_window = &host_window.open_overlay<GUI::ToolWindow>();
        (void)tool_window->load_from_eml_string(eml_editor->content().encode());
        tool_window->set_position({ host_window.size().x() * 3 / 4 - tool_window->size().x() / 2, host_window.size().y() / 2 - tool_window->size().y() / 2 });
        tool_window->on_close = [&tool_window]() {
            tool_window = nullptr;
        };
    };

    app.run();
    return 0;
}
