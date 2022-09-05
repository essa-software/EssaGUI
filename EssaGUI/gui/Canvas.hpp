#pragma once

#include "EssaGUI/gui/Container.hpp"
#include "EssaGUI/gui/sprites/Sprite.hpp"
#include <vector>
namespace GUI {

class Canvas : public Container {
public:
    virtual void on_add() override { set_layout<BasicLayout>(); }

    template<class T, class... Args>
    requires(std::is_base_of_v<Sprite, T>&& requires(Args&&... args) { T(std::forward<Args>(args)...); })
        T* add_sprite(Args&&... args) {
        auto sprite = std::make_shared<T>(std::forward<Args>(args)...);
        m_widgets.push_back(sprite);
        m_sprites.push_back(sprite.get());
        if (get_layout())
            get_layout()->m_multipliers.push_back(1);
        set_needs_relayout();
        return sprite.get();
    }

private:
    std::vector<Sprite*> m_sprites;
};

}
