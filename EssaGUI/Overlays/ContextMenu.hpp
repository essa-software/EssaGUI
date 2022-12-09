#pragma once

#include <EssaGUI/Overlay.hpp>
#include <EssaGUI/Widgets/Widget.hpp>
#include <EssaUtil/UString.hpp>

namespace GUI {

class MenuWidget : public Widget {
public:
    std::function<void(size_t)> on_action;

    void add_action(Util::UString);
    Util::Vector2f wanted_size() const;

private:
    std::vector<Util::UString> m_actions;

    Util::Rectf item_rect(size_t index) const;
    virtual void draw(Gfx::Painter&) const override;
    virtual void handle_event(Event& event) override;
};

class ContextMenu {
public:
    CREATE_VALUE(Util::UString, title, "")

    void add_action(Util::UString label, std::function<void()> callback) {
        m_actions.push_back({ label, callback });
    }

    std::vector<std::pair<Util::UString, std::function<void()>>> const& actions() const { return m_actions; }

private:
    std::vector<std::pair<Util::UString, std::function<void()>>> m_actions;
};

class ContextMenuOverlay : public Overlay {
public:
    ContextMenuOverlay(HostWindow&, ContextMenu, Util::Vector2f position);

    virtual Util::Vector2f position() const override { return m_position; }
    virtual Util::Vector2f size() const override;

private:
    virtual void handle_event(llgl::Event) override;
    virtual void draw(Gfx::Painter&) override;

    MenuWidget* m_menu_widget = nullptr;
    ContextMenu m_context_menu;
    Util::Vector2f m_position;
};

}