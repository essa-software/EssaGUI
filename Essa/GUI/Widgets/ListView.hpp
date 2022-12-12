#pragma once

#include "AbstractListView.hpp"
#include "ScrollableWidget.hpp"
#include <Essa/GUI/Model.hpp>
#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <EssaUtil/Vector.hpp>
#include <functional>
#include <memory>

namespace GUI {

class ListView : public AbstractListView {
public:
    virtual void draw(Gfx::Painter&) const override;
    virtual EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&) override;

    std::function<void(unsigned)> on_click;
    std::function<std::optional<ContextMenu>(unsigned)> on_context_menu_request;
};

}
