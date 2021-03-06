#pragma once

#include "ContextMenu.hpp"
#include "Model.hpp"
#include "ScrollableWidget.hpp"

#include <EssaUtil/Vector.hpp>
#include <functional>
#include <memory>

namespace GUI {

class ListView : public ScrollableWidget {
public:
    explicit ListView(Container& parent)
        : ScrollableWidget(parent) { }

    void set_model(std::unique_ptr<Model> model) { m_model = std::move(model); }

    template<class T, class... Args>
    T& create_and_set_model(Args&&... args) {
        auto model = std::make_unique<T>(std::forward<Args>(args)...);
        auto model_ptr = model.get();
        set_model(std::move(model));
        return *model_ptr;
    }

    virtual void draw(GUI::Window&) const override;
    virtual void handle_event(Event&) override;

    std::function<void(unsigned)> on_click;
    std::function<std::optional<ContextMenu>(unsigned)> on_context_menu_request;

    Util::Vector2f row_position(unsigned) const;

private:
    virtual float content_height() const override;
    Util::Vector2f cell_size(size_t row, size_t column) const;

    std::unique_ptr<Model> m_model;
};

}
