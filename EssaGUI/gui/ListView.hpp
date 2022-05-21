#pragma once

#include "Widget.hpp"
#include <functional>

namespace GUI {

class Model {
public:
    virtual ~Model() = default;

    virtual size_t row_count() const = 0;
    virtual size_t column_count() const = 0;
    virtual std::string data(size_t row, size_t column) const = 0;

    struct Column {
        float width = 15;
        std::string name;
    };

    virtual Column column(size_t column) const = 0;
};

class ListView : public Widget {
public:
    explicit ListView(Container& parent)
        : Widget(parent) { }

    void set_model(std::unique_ptr<Model> model) { m_model = std::move(model); }

    template<class T, class... Args>
    T& create_and_set_model(Args&&... args) {
        auto model = std::make_unique<T>(std::forward<Args>(args)...);
        auto model_ptr = model.get();
        set_model(std::move(model));
        return *model_ptr;
    }

    virtual void draw(sf::RenderWindow&) const override;
    virtual void do_handle_event(Event&) override;

    std::function<void(unsigned)> on_click;

private:
    sf::Vector2f cell_size(size_t row, size_t column) const;

    std::unique_ptr<Model> m_model;
};

}
