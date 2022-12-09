#pragma once

#include "ScrollableWidget.hpp"
#include <EssaGUI/Model.hpp>
#include <EssaGUI/Widgets/Widget.hpp>

namespace GUI {

class AbstractListView : public ScrollableWidget {
public:
    Util::Vector2f row_position(size_t) const;

    void set_model(std::unique_ptr<Model> model) { m_model = std::move(model); }

    template<class T2, class... Args>
    requires(std::is_base_of_v<Model, T2>)
        T2& create_and_set_model(Args&&... args) {
        auto model = std::make_unique<T2>(std::forward<Args>(args)...);
        auto model_ptr = model.get();
        set_model(std::move(model));
        return *model_ptr;
    }

    CREATE_VALUE(bool, display_header, true);

    Model& model() const {
        assert(m_model);
        return *m_model;
    }

protected:
    virtual Util::Vector2f content_size() const override;
    float row_width() const;
    Util::Vector2f cell_size(size_t row, size_t column) const;

private:
    std::unique_ptr<Model> m_model;
};

}
