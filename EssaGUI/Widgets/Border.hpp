#pragma once

#include "EssaGUI/Widgets/Widget.hpp"
#include <EssaGUI/Widgets/Container.hpp>

namespace GUI {

class Border : public Container {
public:
    virtual void on_init() override {
        set_layout<HorizontalBoxLayout>();
    }

    template<class T, class... Args>
    requires(std::is_base_of_v<Widget, T>&& requires(Args&&... args) { T(std::forward<Args>(args)...); })
        T* add_child(Args&&... args) {

        m_widgets.clear();

        auto child = add_widget<T>(std::forward<Args>(args)...);
        child->set_size({ Util::Length::Auto, Util::Length::Auto });

        return child;
    }

    Widget* get_child(){
        if(widgets().size() > 0)
            return widgets().front().get();
        return nullptr;
    }

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

protected:
    using Container::add_widget;
    using Container::add_created_widget;
    using Container::clear_layout;
    using Container::get_layout;
    using Container::shrink;
    using Container::widgets;
};

}
