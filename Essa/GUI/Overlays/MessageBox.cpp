#include "MessageBox.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/ImageWidget.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

namespace GUI {

MessageBox::MessageBox(WidgetTreeRoot& wnd, Util::UString message, Util::UString title, Settings settings)
    : WindowRoot(wnd) {

    auto& prompt_container = set_main_widget<GUI::Container>();
    auto& prompt_container_layout = prompt_container.set_layout<GUI::VerticalBoxLayout>();
    prompt_container_layout.set_spacing(20);
    prompt_container_layout.set_padding(Boxi::all_equal(20));

    constexpr unsigned IMAGE_SIZE = 48;

    auto* content_container = prompt_container.add_widget<GUI::Container>();
    {
        content_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);

        unsigned x_padding = 40;

        // This is a bit awkward, because we require prompt textfield to know
        // window size, we need window size for setting up window, which gives
        // OpenGL context required to load image. So we add image widget in 2 phases,
        // first adds an empty widget, second loads&sets the image after setting up
        // a window.
        // FIXME: Really, user shouldn't think about this.

        ImageWidget* image = nullptr;
        if (settings.icon != Icon::None) {
            image = content_container->add_widget<GUI::ImageWidget>();
            image->set_size({ { IMAGE_SIZE, Util::Length::Px }, Util::Length::Auto });
            x_padding += IMAGE_SIZE + 10; // 48-image width + 10-spacing
        }

        auto* prompt_text = content_container->add_widget<GUI::Textfield>();
        prompt_text->set_content(Gfx::RichText::parse(message));
        prompt_text->set_size({ Util::Length::Auto, Util::Length::Auto });

        {
            auto text_size = prompt_text->needed_size_for_text();
            text_size.set_y(std::max<unsigned>(IMAGE_SIZE + 10, text_size.y()));
            Util::Size2u total_size { x_padding + text_size.x(), 82 + text_size.y() };
            window().setup(std::move(title), total_size, {});
            window().center_on_screen();
        }

        if (settings.icon != Icon::None) {
            std::string image_name = [&]() {
                switch (settings.icon) {
                case Icon::None:
                    ESSA_UNREACHABLE;
                case Icon::Info:
                    return "info";
                case Icon::Warning:
                    return "warning";
                case Icon::Error:
                    return "error";
                }
                ESSA_UNREACHABLE;
            }();
            image->set_image(&GUI::Application::the().resource_manager().require_texture("gui/message-box/" + image_name + ".png"));
        }
    }

    auto* button_container = prompt_container.add_widget<GUI::Container>();
    auto& layout = button_container->set_layout<GUI::HorizontalBoxLayout>();
    layout.set_spacing(20);
    layout.set_content_alignment(BoxLayout::ContentAlignment::BoxEnd);
    button_container->set_size({ Util::Length::Auto, Util::Length::Initial });

    auto add_button = [this, &button_container](ButtonRole button_role, Util::UString label, Util::Color bg_color) {
        auto* button = button_container->add_widget<GUI::TextButton>();
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

    auto const& theme = GUI::Application::the().theme();

    if (settings.buttons == Buttons::YesNo) {
        m_default_button = add_button(ButtonRole::Yes, "Yes", theme.positive);
        add_button(ButtonRole::No, "No", theme.negative);
    }
    else if (settings.buttons == Buttons::Ok) {
        m_default_button = add_button(ButtonRole::Ok, "Ok", theme.neutral);
    }
}

Widget::EventHandlerResult MessageBox::handle_event(llgl::Event const& event) {
    if (auto const* keypress = event.get<Event::KeyPress>()) {
        if (keypress->code() == llgl::KeyCode::Enter && m_default_button) {
            m_default_button->on_click();
            return Widget::EventHandlerResult::Accepted;
        }
        if (keypress->code() == llgl::KeyCode::Escape) {
            close();
            return Widget::EventHandlerResult::Accepted;
        }
    }
    return Widget::EventHandlerResult::NotAccepted;
}

MessageBox::ButtonRole message_box(HostWindow& host_window, Util::UString message, Util::UString title, MessageBox::Buttons buttons) {
    return message_box(&host_window, std::move(message), std::move(title), { .buttons = buttons });
}

MessageBox::ButtonRole message_box(HostWindow* host_window, Util::UString message, Util::UString title, MessageBox::Settings settings) {
    auto msgbox = GUI::Application::the().open_host_window<GUI::MessageBox>(std::move(message), std::move(title), settings);
    msgbox.window.set_always_on_top();
    msgbox.window.show_modal(host_window);
    return msgbox.root.clicked_button();
}

}
