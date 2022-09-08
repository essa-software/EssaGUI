#include "Prompt.hpp"

#include "Application.hpp"
#include "Container.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"

namespace GUI {

Prompt::Prompt(GUI::Window& wnd, Util::UString help_text, Util::UString window_title, Util::UString placeholder)
    : ToolWindow(wnd, "Prompt") {
    set_title(std::move(window_title));
    set_size({ 500, 100 });
    center_on_screen();

    auto& container = set_main_widget<GUI::Container>();
    auto& container_layout = container.set_layout<GUI::VerticalBoxLayout>();
    container_layout.set_padding(Boxf::all_equal(20));
    container_layout.set_spacing(10);

    auto input_container = container.add_widget<GUI::Container>();
    input_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    input_container->set_size({ Length::Auto, 30.0_px });

    {
        auto label = input_container->add_widget<GUI::Textfield>();
        label->set_size({ 150.0_px, Length::Auto });
        label->set_content(help_text);
    }
    auto input = input_container->add_widget<GUI::Textbox>();
    // FIXME: Why textboxes are numeric by default
    input->set_data_type(GUI::Textbox::TEXT);
    input->set_placeholder(std::move(placeholder));
    input->set_focused();

    // FIXME: Also, why buttons are red by default?
    auto button_container = container.add_widget<GUI::Container>();
    button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    button_container->set_size({ Length::Auto, 30.0_px });
    {
        auto cancel_button = button_container->add_widget<GUI::TextButton>();
        cancel_button->set_alignment(GUI::Align::Center);
        cancel_button->set_content("Cancel");
        cancel_button->on_click = [this]() {
            close();
        };

        auto ok_button = button_container->add_widget<GUI::TextButton>();
        ok_button->set_alignment(GUI::Align::Center);
        ok_button->set_content("OK");
        ok_button->on_click = [this, input]() {
            m_result = input->get_content();
            close();
        };
    }
};

std::optional<Util::UString> prompt(Util::UString help_text, Util::UString window_title, Util::UString placeholder) {
    auto& prompt = GUI::Application::the().open_overlay<Prompt>(std::move(help_text), std::move(window_title), std::move(placeholder));
    prompt.run();
    return prompt.result();
}

}
