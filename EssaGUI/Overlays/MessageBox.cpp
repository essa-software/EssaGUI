#include "MessageBox.hpp"

#include <EssaGUI/Application.hpp>
#include <EssaGUI/Graphics/Text.hpp>
#include <EssaGUI/Overlays/ToolWindow.hpp>
#include <EssaGUI/Widgets/Container.hpp>
#include <EssaGUI/Widgets/TextButton.hpp>
#include <EssaGUI/Widgets/Textfield.hpp>

namespace GUI {

MessageBox::MessageBox(HostWindow& window, Util::UString message, Util::UString title, Buttons buttons)
    : ToolWindow(window, "MessageBox") {
    set_title(std::move(title));

    auto& prompt_container = set_main_widget<GUI::Container>();

    auto& prompt_container_layout = prompt_container.set_layout<GUI::VerticalBoxLayout>();
    prompt_container_layout.set_spacing(20);
    prompt_container_layout.set_padding(Boxf::all_equal(20));

    auto prompt_text = prompt_container.add_widget<GUI::Textfield>();
    prompt_text->set_content(std::move(message));
    prompt_text->set_alignment(GUI::Align::Center);

    {
        Gfx::Text text { prompt_text->content(), GUI::Application::the().font() };
        text.set_font_size(prompt_text->font_size());
        auto text_size = text.calculate_text_size();
        Util::Vector2f total_size { 40 + text_size.x(), 110 + text_size.y() };
        set_size(total_size);
        center_on_screen();
    }

    auto button_container = prompt_container.add_widget<GUI::Container>();
    button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    button_container->set_size({ Util::Length::Auto, 30.0_px });

    auto add_button = [this, &button_container](ButtonRole button_role, Util::UString label, Util::Color bg_color) {
        auto button = button_container->add_widget<GUI::TextButton>();
        button->set_alignment(GUI::Align::Center);
        button->set_content(std::move(label));

        button->override_button_colors().normal.unhovered.background = bg_color;
        // TODO: Take this from theme somehow?
        button->override_button_colors().normal.hovered.background = bg_color + Util::Color { 20, 20, 20 };

        button->on_click = [this, button_role]() {
            m_clicked_button = button_role;
            close();
        };
        return button;
    };

    if (buttons == Buttons::YesNo) {
        m_default_button = add_button(ButtonRole::Yes, "Yes", theme().positive);
        add_button(ButtonRole::No, "No", theme().negative);
    }
    else if (buttons == Buttons::Ok) {
        m_default_button = add_button(ButtonRole::Ok, "Ok", theme().neutral);
    }
}

void MessageBox::handle_event(llgl::Event event) {
    ToolWindow::handle_event(event);
    if (event.type == llgl::Event::Type::KeyPress && event.key.keycode == llgl::KeyCode::Enter && m_default_button)
        m_default_button->on_click();
}

MessageBox::ButtonRole message_box(HostWindow& window, Util::UString message, Util::UString title, MessageBox::Buttons buttons) {
    auto& msgbox = window.open_overlay<GUI::MessageBox>(std::move(message), std::move(title), buttons);
    return msgbox.exec();
}

}
