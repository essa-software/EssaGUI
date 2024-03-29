#include "Prompt.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

namespace GUI {

Prompt::Prompt(WidgetTreeRoot& window, Util::UString help_text, Util::UString window_title, Util::UString placeholder)
    : WindowRoot(window) {
    window.setup(std::move(window_title), { 500, 120 }, {});
    window.center_on_screen();

    auto& container = set_main_widget<GUI::Container>();
    auto& container_layout = container.set_layout<GUI::VerticalBoxLayout>();
    container_layout.set_padding(Boxi::all_equal(20));
    container_layout.set_spacing(10);

    auto* input_container = container.add_widget<GUI::Container>();
    input_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    input_container->set_size({ Util::Length::Auto, Util::Length::Auto });

    {
        auto* label = input_container->add_widget<GUI::Textfield>();
        label->set_content(help_text);
    }
    auto* input = input_container->add_widget<GUI::Textbox>();
    // FIXME: Why textboxes are numeric by default
    input->set_type(GUI::Textbox::TEXT);
    input->set_placeholder(std::move(placeholder));
    input->set_focused();

    // FIXME: Also, why buttons are red by default?
    auto* button_container = container.add_widget<GUI::Container>();
    button_container->set_size({ Util::Length::Auto, Util::Length::Initial });
    auto& layout = button_container->set_layout<GUI::HorizontalBoxLayout>();
    layout.set_spacing(20);
    layout.set_content_alignment(BoxLayout::ContentAlignment::BoxEnd);
    {
        m_cancel_button = button_container->add_widget<GUI::TextButton>();
        m_cancel_button->set_alignment(GUI::Align::Center);
        m_cancel_button->set_content("Cancel");
        m_cancel_button->on_click = [this]() { close(); };

        m_ok_button = button_container->add_widget<GUI::TextButton>();
        m_ok_button->set_alignment(GUI::Align::Center);
        m_ok_button->set_content("OK");
        m_ok_button->on_click = [this, input]() {
            m_result = input->content();
            close();
        };
    }
}

Widget::EventHandlerResult Prompt::handle_event(llgl::Event const& event) {
    if (auto const* keypress = event.get<Event::KeyPress>()) {
        if (keypress->code() == llgl::KeyCode::Enter) {
            m_ok_button->on_click();
            return Widget::EventHandlerResult::Accepted;
        }
        if (keypress->code() == llgl::KeyCode::Escape) {
            m_cancel_button->on_click();
            return Widget::EventHandlerResult::Accepted;
        }
    }
    return Widget::EventHandlerResult::NotAccepted;
}

std::optional<Util::UString> prompt(HostWindow& wnd, Util::UString help_text, Util::UString window_title, Util::UString placeholder) {
    auto prompt = GUI::Application::the().open_host_window<Prompt>(std::move(help_text), std::move(window_title), std::move(placeholder));
    prompt.window.show_modal(&wnd);
    return prompt.root.result();
}

}
