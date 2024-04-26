#pragma once

#include "ScrollableWidget.hpp"
#include <Essa/GUI/Model.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

namespace GUI {

class AbstractListView : public ScrollableWidget {
public:
    // Calculate row position for index, in widget-local coords.
    Util::Point2i row_position(size_t) const;

    void set_model(std::unique_ptr<Model> model) {
        m_model = std::move(model);
    }

    template<class T2, class... Args>
        requires(std::is_base_of_v<Model, T2>)
    T2& create_and_set_model(Args&&... args) {
        auto model = std::make_unique<T2>(std::forward<Args>(args)...);
        auto model_ptr = model.get();
        set_model(std::move(model));
        return *model_ptr;
    }

    CREATE_VALUE(bool, display_header, true);

    Model* model() const {
        return m_model.get();
    }

protected:
    virtual Util::Size2i content_size() const override;
    float row_width() const;
    Util::Size2i cell_size(size_t row, size_t column) const;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader&) override;

private:
    std::unique_ptr<Model> m_model;
};

}
