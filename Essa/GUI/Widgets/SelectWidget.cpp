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
    m_textbox->on_change = [this](auto const&) {
        if (m_search_enabled) {
            show_or_render_select_overlay();
            // Note: Don't switch focus.
            host_window().request_focus();
        }
    };

    auto* expand_button = add_widget<GUI::ArrowButton>();
    auto line_height = Util::Length(theme().line_height, Util::Length::Px);
    expand_button->set_size({ line_height, line_height });
    expand_button->set_arrow_type(GUI::ArrowButton::ArrowType::BOTTOMARROW);
    expand_button->on_click = [this]() {
        show_or_render_select_overlay();
        m_select_overlay->request_focus();
        m_select_overlay->set_captures_mouse();
    };
}

void SelectWidget::set_search_enabled(bool enabled) {
    m_search_enabled = enabled;
    m_textbox->set_editable(enabled);
}

void SelectWidget::show_or_render_select_overlay() {
    if (m_select_overlay) {
        render_select_overlay();
        return;
    }

    auto option_count = std::ranges::count_if(m_options, [this](auto const& option) {
        return !m_search_enabled || option.label.contains(m_textbox->content());
    });

    auto& select_overlay = GUI::Application::the().create_host_window(
        { raw_size().x(), option_count * theme().line_height }, "Essa GUI::SelectWidget overlay", { .flags = llgl::WindowFlags::PopupMenu }
    );

    select_overlay.on_event = [&](llgl::Event const& event) {
        if (event.is<llgl::Event::WindowFocusLost>()) {
            select_overlay.close();
            return Widget::EventHandlerResult::Accepted;
        }
        return Widget::EventHandlerResult::NotAccepted;
    };
    select_overlay.set_position(this->screen_position() + Util::Vector2i(0, this->raw_size().y()));
    m_select_overlay = &select_overlay;
    render_select_overlay();
    select_overlay.on_close = [&]() { m_select_overlay = nullptr; };
}

void SelectWidget::render_select_overlay() {
    if (!m_select_overlay || !m_textbox) {
        return;
    }
    auto& root = m_select_overlay->set_root_widget<GUI::Container>();
    root.set_layout<GUI::VerticalBoxLayout>();

    auto search_query = m_textbox->content();

    for (auto const& option : m_options) {
        if (m_search_enabled && !option.label.contains(search_query) && option.name.find(search_query.encode()) == std::string::npos) {
            continue;
        }

        auto& button = *root.add_widget<GUI::TextButton>();
        button.set_image(&resource_manager().require_texture("gui/newFile.png"));
        button.set_content(option.label);
        button.set_alignment(GUI::Align::CenterLeft);
        button.on_click = [this, idx = &option - m_options.data()]() {
            set_selected_index(idx);
            m_select_overlay->close();
        };
    }
    m_select_overlay->set_size({ std::max<float>(raw_size().x(), root.initial_size().x.value()), m_select_overlay->size().y() });
    for (auto const& btn : root.widgets()) {
        btn->set_size({ { static_cast<float>(m_select_overlay->size().x()), Util::Length::Px }, Util::Length::Initial });
    }
}

void SelectWidget::clear_options() {
    m_options.clear();
    m_selected_index = 0;
    if (m_textbox) {
        m_textbox->set_content("", NotifyUser::No);
    }
}

void SelectWidget::add_option(SelectOption option) {
    m_options.push_back(std::move(option));
}

void SelectWidget::set_selected_index(std::optional<unsigned> idx, NotifyUser notify_user) {
    m_selected_index = idx;
    if (m_textbox) {
        m_textbox->set_content(idx ? m_options[*idx].label : "", NotifyUser::No);
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

    set_search_enabled(TRY(object.get_property("search_enabled", EML::Value(m_search_enabled)).to<bool>()));

    return {};
}

EML_REGISTER_CLASS(SelectWidget)

}
