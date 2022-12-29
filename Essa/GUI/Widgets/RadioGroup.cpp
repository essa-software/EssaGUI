#include "RadioGroup.hpp"

#include "RadioButton.hpp"

namespace GUI {

void RadioGroup::on_init() {
    set_layout<VerticalBoxLayout>().set_spacing(5);
}

void RadioGroup::update() {
    size_t i = 0;
    size_t temp = m_index;

    for (const auto& radio : m_buttons) {
        if (radio->is_active() && i != m_index) {
            temp = i;
            radio->set_active(true);
        }
        else {
            radio->set_active(false);
        }

        i++;
    }

    if (temp != m_index) {
        m_index = temp;

        if (on_change)
            on_change(m_index);
    }

    if (m_index < m_buttons.size())
        m_buttons[m_index]->set_active(true);
}

void RadioGroup::set_index(size_t index) {
    m_index = index;

    size_t i = 0;

    if (on_change)
        on_change(m_index);

    for (const auto& radio : m_buttons) {
        if (i == m_index)
            radio->set_active(true);
        else
            radio->set_active(false);
        i++;
    }
}

void RadioGroup::add_radio(const Util::UString& caption) {
    m_buttons.push_back(add_widget<RadioButton>());
    m_buttons.back()->set_size({ Util::Length::Auto, m_row_height });
    m_buttons.back()->set_caption(caption);
}

EML::EMLErrorOr<void> RadioGroup::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    // Intentionally calling Widget to disallow setting Container properties from EML.
    TRY(Widget::load_from_eml_object(object, loader));

    for (auto const& child : object.objects) {
        // FIXME: This is hacky. Doesn't even copy all RadioButton properties.
        std::shared_ptr<RadioButton> button = TRY(child.construct<RadioButton>(loader, widget_tree_root()));
        assert(button);
        add_radio(button->caption());
    }
    return {};
}

EML_REGISTER_CLASS(RadioGroup);

}
