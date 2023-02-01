#pragma once

#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/RadioButton.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/Is.hpp>
#include <EssaUtil/UString.hpp>
#include <functional>
#include <vector>

namespace GUI {

// Container that handles mutual exclusivity of RadioButtons.
// Note: RadioButtons must be added as direct children, otherwise
// they won't work as expected.
class RadioGroup : public Container {
public:
    template<class T, class... Args>
        requires(std::is_base_of_v<Widget, T> && requires(Args && ... args) { T(std::forward<Args>(args)...); })
    T* add_widget(Args&&... args) {
        auto widget = Container::add_widget<T>(std::forward<Args>(args)...);
        if (Util::is<RadioButton>(*widget)) {
            add_radio_button(static_cast<RadioButton&>(*widget));
        }
        return widget;
    }
    void add_created_widget(std::shared_ptr<Widget> widget) {
        Container::add_created_widget(widget);
        if (Util::is<RadioButton>(*widget.get())) {
            add_radio_button(static_cast<RadioButton&>(*widget));
        }
    }

    void set_index(size_t index);
    size_t get_index() const { return m_index; }

    std::function<void(size_t)> on_change;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    void add_radio_button(RadioButton&);

    size_t m_index = 0;
    std::vector<RadioButton*> m_radio_buttons;

protected:
    using Container::add_created_widget;
    using Container::add_widget;
    using Container::get_layout;
    using Container::shrink;
};

}
