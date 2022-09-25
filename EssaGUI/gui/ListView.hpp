#pragma once

#include "AbstractListView.hpp"
#include "ContextMenu.hpp"
#include "Model.hpp"
#include "ScrollableWidget.hpp"

#include <EssaUtil/Vector.hpp>
#include <functional>
#include <memory>

namespace GUI {

class ListView : public AbstractListView {
public:
    virtual void draw(Gfx::Painter&) const override;
    virtual void handle_event(Event&) override;

    std::function<void(unsigned)> on_click;
    std::function<std::optional<ContextMenu>(unsigned)> on_context_menu_request;
};

}
