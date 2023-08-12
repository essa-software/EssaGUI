#include "Prompt.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

namespace GUI {

Prompt::Prompt(WidgetTreeRoot& window, Util::UString help_text, Util::UString window_title, Util::UString placeholder)
    : WindowRoot(window) {
    window.setup(std::move(window_title), { 500, 110 }, {});
    window.center_on_screen();

    auto& container = set_main_widget<GUI::Container>();
    auto& container_layout = container.set_layout<GUI::VerticalBoxLayout>();
    container_layout.set_padding(Boxi::all_equal(20));
    container_layout.set_spacing(10);

    auto* input_container = container.add_widget<GUI::Container>();
    input_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    input_container->set_size({ Util::Length::Auto, 32.0_px });

    {
        auto* label = input_container->add_widget<GUI::Textfield>();
        label->set_size({ 150.0_px, Util::Length::Auto });
        label->set_content(help_text);
    }
    auto* input = input_container->add_widget<GUI::Textbox>();
    // FIXME: Why textboxes are numeric by default
    input->set_type(GUI::Textbox::TEXT);
    input->set_placeholder(std::move(placeholder));
    input->set_focused();

    // FIXME: Also, why buttons are red by default?
    auto* button_container = container.add_widget<GUI::Container>();
    button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    button_container->set_size({ Util::Length::Auto, 32.0_px });
    {
        auto* cancel_button = button_container->add_widget<GUI::TextButton>();
        cancel_button->set_alignment(GUI::Align::Center);
        cancel_button->set_content("Cancel");
        cancel_button->on_click = [this]() { close(); };

        auto* ok_button = button_container->add_widget<GUI::TextButton>();
        ok_button->set_alignment(GUI::Align::Center);
        ok_button->set_content("OK");
        ok_button->on_click = [this, input]() {
            m_result = input->content();
            close();
        };
    }
};

std::optional<Util::UString> prompt(HostWindow&, Util::UString help_text, Util::UString window_title, Util::UString placeholder) {
    auto prompt = GUI::Application::the().open_host_window<Prompt>(std::move(help_text), std::move(window_title), std::move(placeholder));
    prompt.window.show_modal();
    return prompt.root.result();
}

}
