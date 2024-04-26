#pragma once

#include <Essa/GUI/Sprites/Sprite.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <vector>

namespace GUI {

class Canvas : public Container {
public:
    virtual void on_init() override {
        set_layout<BasicLayout>();
    }

    template<class T, class... Args>
        requires(std::is_base_of_v<Sprite, T> && requires(Args&&... args) { T(std::forward<Args>(args)...); })
    T* add_sprite(Args&&... args) {
        auto sprite = std::make_shared<T>(std::forward<Args>(args)...);
        m_widgets.push_back(sprite);
        m_sprites.push_back(sprite.get());
        set_needs_relayout();
        return sprite.get();
    }

    void draw_rect(LengthVector pos, LengthVector size);

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    std::vector<Sprite*> m_sprites;

protected:
    using Container::add_created_widget;
    using Container::add_widget;
    using Container::clear_layout;
    using Container::get_layout;
    using Container::shrink;
    using Container::widgets;
};

}
