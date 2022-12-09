#include "Sprite.hpp"

#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/Window/Event.hpp>

namespace GUI {

void Sprite::handle_event(Event& event) {
    if (event.event().type == llgl::Event::Type::MouseButtonPress && is_hover()) {
        if (on_click)
            on_click();
    }
}

}
