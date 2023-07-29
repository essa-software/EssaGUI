#pragma once

#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Reactivity.hpp>
#include <Essa/GUI/Widgets/ButtonBehavior.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <functional>
#include <optional>

namespace GUI {

class Button : public Widget {
public:
    Button();

    std::function<void()> on_click;

    virtual void draw(Gfx::Painter& window) const override = 0;

    ReadOnlyObservable<bool> active() const { return m_active; }
    bool is_active() const { return m_active.get(); }

    void set_active(bool active, NotifyUser notify_user = NotifyUser::Yes) {
        if (m_active.get() != active) {
            m_active = active;
            if (notify_user == NotifyUser::Yes && on_change)
                on_change(active);
        }
    }

    CREATE_BOOLEAN(toggleable, false)

    std::function<void(bool)> on_change;

    // TODO: Replace this with full theme override system
    virtual Theme::ButtonColors& override_button_colors() {
        if (m_button_colors_override)
            return *m_button_colors_override;
        m_button_colors_override = default_button_colors();
        return *m_button_colors_override;
    }

    Theme::BgFgTextColors colors_for_state() const;

protected:
    virtual Theme::ButtonColors default_button_colors() const = 0;

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;
    virtual bool accepts_focus() const override { return true; }

private:
    virtual Widget::EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&) override;
    virtual Widget::EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const&) override;
    virtual Widget::EventHandlerResult on_mouse_move(Event::MouseMove const& event) override;
    virtual Widget::EventHandlerResult on_key_press(Event::KeyPress const&) override;
    void click();

    std::optional<Theme::ButtonColors> m_button_colors_override;

    Observable<bool> m_active { false };
    ButtonBehavior m_behavior;
};

}
