#pragma once

#include "EssaGUI/gui/Container.hpp"
namespace GUI{

class Border : public Container{
public:
    Border(Container& c) : GUI::Container(c){
        set_layout<HorizontalBoxLayout>();
    }

    template<class T, class... Args>
    requires(std::is_base_of_v<Widget, T>&& requires(Container& c, Args&&... args) { T(c, std::forward<Args>(args)...); })
    T* add_child(Args&&... args){
        
        m_widgets.clear();
        
        auto child = add_widget<T>(std::forward<Args>(args)...);
        child->set_size({Length::Auto, Length::Auto});

        return child;
    } 
};

}
