#include "Canvas.hpp"

#include <Essa/GUI/Sprites/RectangleShape.hpp>

namespace GUI {

void Canvas::draw_rect(LengthVector pos, LengthVector size) {
    auto sprite = add_sprite<RectangleShape>();
    sprite->set_position(pos);
    sprite->set_size(size);
}

EML::EMLErrorOr<void> Canvas::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    for (auto const& child : object.objects) {
        std::shared_ptr<Sprite> sprite = TRY(child.construct<Sprite>(loader));
        assert(sprite);
        add_created_widget(std::move(sprite));
        m_sprites.push_back(sprite.get());
    }

    return {};
}

EML_REGISTER_CLASS(Canvas);

}
