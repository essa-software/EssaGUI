#include "Sprite.hpp"

#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/Window/Event.hpp>

namespace GUI {

Widget::EventHandlerResult Sprite::on_mouse_button_press(Event::MouseButtonPress const&) {
    if (on_click)
        on_click();
    return Widget::EventHandlerResult::NotAccepted;
}

}
