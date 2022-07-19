#pragma once

#include "Overlay.hpp"

namespace GUI {

class MenuWidget : public Widget {
public:
    explicit MenuWidget(WidgetTreeRoot& parent)
        : Widget(parent) { }

    explicit MenuWidget(Container& parent)
        : Widget(parent) { }

    std::function<void(Util::UString const&)> on_action;

    void add_action(Util::UString);
    Util::Vector2f wanted_size() const;

private:
    std::vector<Util::UString> m_actions;

    Util::Rectf item_rect(size_t index) const;
    virtual void draw(Window&) const override;
    virtual void handle_event(Event& event) override;
};

class ContextMenu {
public:
    void add_action(Util::UString label, std::function<void()> callback) {
        m_actions.push_back({ label, callback });
    }

    std::vector<std::pair<Util::UString, std::function<void()>>> const& actions() const { return m_actions; }

private:
    std::vector<std::pair<Util::UString, std::function<void()>>> m_actions;
};

class ContextMenuOverlay : public Overlay {
public:
    ContextMenuOverlay(GUI::Window& wnd, ContextMenu, Util::Vector2f position);

    virtual Util::Vector2f position() const override { return m_position; }
    virtual Util::Vector2f size() const override {
        return m_menu_widget->wanted_size();
    }

private:
    void add_action(Util::UString label, std::function<void()> callback);
    virtual void handle_event(llgl::Event) override;
    virtual void handle_events() override;

    MenuWidget* m_menu_widget = nullptr;
    std::map<Util::UString, std::function<void()>> m_actions;
    Util::Vector2f m_position;
};

}
