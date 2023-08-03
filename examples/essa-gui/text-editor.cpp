#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/TextEditing/SyntaxHighlighter.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>

class TestSyntaxHighlighter : public GUI::SyntaxHighlighter {
    virtual std::vector<GUI::TextStyle> styles() const override {
        return {
            GUI::TextStyle { .color = GUI::Application::the().theme().positive },
            GUI::TextStyle { .color = GUI::Application::the().theme().negative },
        };
    }

    virtual std::vector<GUI::StyledTextSpan> spans(Util::UString const& input) const override {
        std::vector<GUI::StyledTextSpan> result;
        for (size_t s = 0; s < input.size(); s++) {
            result.push_back(GUI::StyledTextSpan { .span_start = s, .span_size = 1, .style_index = s % 2 });
        }
        return result;
    }
};

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Text Editor");

    auto& container1 = host_window.set_root_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();
    auto text_editor = container1.add_widget<GUI::TextEditor>();
    text_editor->set_placeholder("Test placeholder");
    text_editor->set_syntax_highlighter(std::make_unique<TestSyntaxHighlighter>());

    text_editor->set_error_spans({
        GUI::TextEditor::ErrorSpan { GUI::TextEditor::ErrorSpan::Type::Error,
            { { 0, 0 }, { 0, 10 } } },
        GUI::TextEditor::ErrorSpan { GUI::TextEditor::ErrorSpan::Type::Warning,
            { { 1, 0 }, { 1, 10 } } },
        GUI::TextEditor::ErrorSpan { GUI::TextEditor::ErrorSpan::Type::Note,
            { { 2, 0 }, { 2, 10 } } },
    });

    auto number_text_box = container1.add_widget<GUI::Textbox>();
    number_text_box->set_placeholder("Enter number");
    number_text_box->on_enter = [&host_window](Util::UString const& content) {
        GUI::message_box(host_window, "You entered number: " + content, "Message", GUI::MessageBox::Buttons::Ok);
    };

    auto text_box = container1.add_widget<GUI::Textbox>();
    text_box->set_placeholder("Enter text");
    text_box->set_type(GUI::Textbox::Type::TEXT);
    text_box->on_enter = [&host_window](Util::UString const& content) {
        GUI::message_box(host_window, "You entered text: " + content, "Message", GUI::MessageBox::Buttons::Ok);
    };

    app.run();
    return 0;
}
