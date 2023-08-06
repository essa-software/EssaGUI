#include "NotificationContainer.hpp"

#include <Essa/GUI/Graphics/Text.hpp>

namespace GUI {

void NotificationContainer::spawn_notification(Util::UString message, Level level) {
    m_notifications.push_back(Notification { .message = std::move(message), .level = level });
}

void NotificationContainer::draw_notification(Gfx::Painter& painter, Notification const& notification, float y) const {
    Gfx::Text text { notification.message, resource_manager().font() };
    text.set_font_size(theme().label_font_size);
    text.align(GUI::Align::Top, local_rect().move_y(y).cast<float>());
    switch (notification.level) {
    case Level::Info:
        text.set_fill_color(Util::Colors::Lime);
        break;
    case Level::Error:
        text.set_fill_color(Util::Colors::Red);
        break;
    }
    text.draw(painter);
}

void NotificationContainer::draw(Gfx::Painter& painter) const {
    float y = 0;
    for (auto const& notification : m_notifications) {
        draw_notification(painter, notification, y);
        y += 30;
    }
}

void NotificationContainer::update() {
    for (auto& notification : m_notifications) {
        notification.remaining_ticks--;
    }
    std::erase_if(m_notifications, [](auto const& notification) { return notification.remaining_ticks <= 0; });
}

}
