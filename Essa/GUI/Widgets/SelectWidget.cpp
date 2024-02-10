#include "SelectWidget.hpp"

#include "Textfield.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Widgets/ArrowButton.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/LLGL/Window/WindowSettings.hpp>
#include <EssaUtil/Units.hpp>

namespace GUI {

SelectWidget::~SelectWidget() {
    if (m_select_overlay) {
        m_select_overlay->close();
    }
}

void SelectWidget::on_init() {
    set_layout<GUI::HorizontalBoxLayout>();
    m_textbox = add_widget<GUI::Textbox>();
    m_textbox->set_editable(false);

    auto* expand_button = add_widget<GUI::ArrowButton>();
    auto line_height = Util::Length(theme().line_height, Util::Length::Px);
    expand_button->set_size({ line_height, line_height });
    expand_button->set_arrow_type(GUI::ArrowButton::ArrowType::BOTTOMARROW);
    expand_button->on_click = [this]() {
        // FIXME: Use WindowFlags::PopupMenu here, but it works weird with focus etc.
        auto& select_overlay = GUI::Application::the().create_host_window(
            { raw_size().x(), m_options.size() * theme().line_height }, "Essa GUI::SelectWidget overlay",
            { .flags = llgl::WindowFlags::PopupMenu }
        );
        select_overlay.request_focus();
        select_overlay.set_captures_mouse();

        select_overlay.on_event = [&](llgl::Event const& event) {
            if (event.is<llgl::Event::WindowFocusLost>()) {
                select_overlay.close();
                return Widget::EventHandlerResult::Accepted;
            }
            return Widget::EventHandlerResult::NotAccepted;
        };
        select_overlay.set_position(this->host_position() + Util::Vector2i(0, this->raw_size().y()));
        auto& root = select_overlay.set_root_widget<GUI::Container>();
        root.set_layout<GUI::VerticalBoxLayout>();
        for (auto const& option : m_options) {
            auto& button = *root.add_widget<GUI::TextButton>();
            button.set_image(&resource_manager().require_texture("gui/newFile.png"));
            button.set_content(option.label);
            button.set_alignment(GUI::Align::CenterLeft);
            button.on_click = [this, &select_overlay, idx = &option - m_options.data()]() {
                set_selected_index(idx);
                select_overlay.close();
            };
        }
        select_overlay.on_close = [&]() { m_select_overlay = nullptr; };
        select_overlay.set_size({ std::max<float>(raw_size().x(), root.initial_size().x.value()), select_overlay.size().y() });
        for (auto const& btn : root.widgets()) {
            btn->set_size({ { static_cast<float>(select_overlay.size().x()), Util::Length::Px }, Util::Length::Initial });
        }
        m_select_overlay = &select_overlay;
        select_overlay.show_modal_non_blocking(host_window());
    };
}

void SelectWidget::add_option(SelectOption option) {
    m_options.push_back(std::move(option));
    if (m_options.size() == 1) {
        set_selected_index(0, NotifyUser::No);
    }
}

void SelectWidget::set_selected_index(unsigned idx, NotifyUser notify_user) {
    m_selected_index = idx;
    if (m_textbox) {
        m_textbox->set_content(m_options[idx].label);
    }
    if (notify_user == NotifyUser::Yes && on_change) {
        on_change(idx);
    }
}

struct Option : public EML::EMLObject {
    std::string name;
    Util::UString label;

    EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override {
        name = object.id;
        label = TRY(TRY(object.require_property("label")).to<Util::UString>());
        return {};
    }
};

EML_REGISTER_CLASS(Option)

EML::EMLErrorOr<void> SelectWidget::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    // Deliberately skipping Container so that you don't need (and can't) override things like layout here
    TRY(Widget::load_from_eml_object(object, loader)); // NOLINT(bugprone-parent-virtual-call)

    for (auto const& child : object.objects) {
        std::shared_ptr<Option> option = TRY(child.construct<Option>(loader));
        assert(option);
        add_option({ .name = option->name, .label = option->label });
    }

    return {};
}

EML_REGISTER_CLASS(SelectWidget)

}
