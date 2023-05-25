#pragma once

#include <Essa/GUI/Overlay.hpp>

namespace GUI {

class NotificationWindow : public Overlay {
public:
    explicit NotificationWindow(HostWindow& window)
        : Overlay(window, "NotificationWindow") { }

    enum class Level { Info, Error };
    void spawn_notification(Util::UString message, Level);

private:
    struct Notification {
        int remaining_ticks = 120;
        Util::UString message;
        Level level {};
    };

    void draw_notification(Gfx::Painter&, Notification const&, float y) const;
    virtual void update() override;
    virtual void draw(Gfx::Painter&) override;

    std::vector<Notification> m_notifications;
};

}
