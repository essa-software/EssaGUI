#pragma once

#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/GUI/WindowRoot.hpp>
#include <EssaUtil/UString.hpp>

namespace GUI {

class MenuWidget : public Widget {
public:
    std::function<void(size_t)> on_action;

    void add_action(Util::UString);
    Util::Size2i wanted_size() const;

private:
    std::vector<Util::UString> m_actions;

    Util::Rectf item_rect(size_t index) const;
    virtual void draw(Gfx::Painter&) const override;
    virtual Widget::EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const& event) override;
};

class ContextMenu {
public:
    CREATE_VALUE(Util::UString, title, "")

    void add_action(Util::UString label, std::function<void()> callback) { m_actions.push_back({ label, callback }); }

    std::vector<std::pair<Util::UString, std::function<void()>>> const& actions() const { return m_actions; }

private:
    std::vector<std::pair<Util::UString, std::function<void()>>> m_actions;
};

class ContextMenuOverlay : public WindowRoot {
public:
    ContextMenuOverlay(WidgetTreeRoot&, ContextMenu);

private:
    Util::Size2i required_size() const;
    virtual Widget::EventHandlerResult handle_event(llgl::Event const&) override;

    MenuWidget* m_menu_widget = nullptr;
    ContextMenu m_context_menu;
    Util::Point2i m_position;
};

}
