#include <EssaGUI/EML/SyntaxHighlighter.hpp>
#include <EssaGUI/Application.hpp>
#include <EssaGUI/Widgets/Container.hpp>
#include <EssaGUI/Overlays/MessageBox.hpp>
#include <EssaGUI/Widgets/TextButton.hpp>
#include <EssaGUI/Widgets/TextEditor.hpp>
#include <EssaUtil/Error.hpp>
#include <EssaUtil/GenericParser.hpp>

class EMLPreview : public GUI::Container {
public:
    virtual void on_init() override {
        set_layout<GUI::HorizontalBoxLayout>();

        auto eml_editor = add_widget<GUI::TextEditor>();
        eml_editor->on_change = [this](Util::UString const& text) {
            m_eml_container->shrink(0);
            (void)m_eml_container->load_from_eml_string(text.encode());
        };
        eml_editor->set_syntax_highlighter(std::make_unique<EML::SyntaxHighlighter>());
        m_eml_container = add_widget<GUI::Container>();
    }

private:
    GUI::Container* m_eml_container = nullptr;
};

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "EML test");
    host_window.set_main_widget<EMLPreview>();
    app.run();
    return 0;
}
