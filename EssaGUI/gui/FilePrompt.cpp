#include "FilePrompt.hpp"

#include "Application.hpp"
#include "Container.hpp"
#include "FileExplorer.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToolWindow.hpp"

namespace GUI {

FilePrompt::FilePrompt(HostWindow& window, Util::UString help_text, Util::UString window_title, Util::UString placeholder)
    : ToolWindow(window, "Prompt") {

    set_title(std::move(window_title));
    set_size({ 500, 100 });
    center_on_screen();

    auto& container = set_main_widget<Container>();
    auto& container_layout = container.set_layout<VerticalBoxLayout>();
    container_layout.set_padding(Boxf::all_equal(20));
    container_layout.set_spacing(10);

    auto input_container = container.add_widget<Container>();
    input_container->set_layout<HorizontalBoxLayout>().set_spacing(20);
    input_container->set_size({ Util::Length::Auto, 30.0_px });

    {
        auto label = input_container->add_widget<Textfield>();
        label->set_size({ 160.0_px, Util::Length::Auto });
        label->set_content(help_text);
    }
    auto input = input_container->add_widget<Textbox>();
    // FIXME: Why textboxes are numeric by default
    input->set_type(Textbox::TEXT);
    input->set_placeholder(std::move(placeholder));

    auto file_btn = input_container->add_widget<TextButton>();
    file_btn->set_content("Browse file");

    file_btn->on_click = [this, input, &window]() {
        auto& file_explorer_wnd = window.open_overlay<FileExplorer>();
        file_explorer_wnd.set_size({ 1000, 600 });
        file_explorer_wnd.center_on_screen();

        for (const auto& ext : m_extensions)
            file_explorer_wnd.model()->add_desired_extension(ext);

        file_explorer_wnd.on_submit = [input](std::filesystem::path path) {
            input->set_content(Util::UString { path.string() });
        };

        file_explorer_wnd.run();
    };

    // FIXME: Also, why buttons are red by default?
    auto button_container = container.add_widget<Container>();
    button_container->set_layout<HorizontalBoxLayout>().set_spacing(20);
    button_container->set_size({ Util::Length::Auto, 30.0_px });
    {
        auto cancel_button = button_container->add_widget<TextButton>();
        cancel_button->set_alignment(Align::Center);
        cancel_button->set_content("Cancel");
        cancel_button->on_click = [&]() {
            close();
        };

        auto ok_button = button_container->add_widget<TextButton>();
        ok_button->set_alignment(Align::Center);
        ok_button->set_content("OK");
        ok_button->on_click = [&, input]() {
            m_result = input->content();
            close();
        };
    }
};

FilePrompt* file_prompt(HostWindow& window, Util::UString help_text, Util::UString window_title, Util::UString placeholder) {
    auto& prompt = window.open_overlay<FilePrompt>(std::move(help_text), std::move(window_title), std::move(placeholder));

    prompt.run();

    return &prompt;
}

}
