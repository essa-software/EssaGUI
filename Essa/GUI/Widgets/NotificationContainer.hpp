#pragma once

#include <Essa/GUI/Widgets/Widget.hpp>

namespace GUI {

class NotificationContainer : public Widget {
public:
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
    virtual void draw(Gfx::Painter&) const override;

    std::vector<Notification> m_notifications;
};

}
