#pragma once

#include "Essa/GUI/EML/EMLError.hpp"
#include <Essa/GUI/Widgets/Widget.hpp>

#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <functional>

namespace GUI {

#define __ENUMERATE_LABELLINGS(E) \
    E(Percentage)                 \
    E(Value)                      \
    E(None)

class Progressbar : public Widget {
public:
    virtual void draw(Gfx::Painter& window) const override;

    ESSA_ENUM(Labelling, __ENUMERATE_LABELLINGS)

    Util::UString get_content_with_labelling() const;
    void step_by();

    size_t value() const { return m_value; }
    void set_value(size_t const& val) {
        m_value = val;
        if (m_value >= m_max && on_finish)
            on_finish();
    }

    CREATE_VALUE(Util::UString, content, "")
    CREATE_VALUE(Labelling, labelling, Labelling::None)
    CREATE_VALUE(size_t, min, 0)
    CREATE_VALUE(size_t, max, 100)
    CREATE_VALUE(size_t, step, 1)
    CREATE_VALUE(Util::Color, progressbar_color, Util::Colors::Green)

    std::function<void()> on_finish;

    bool finished() const { return m_value >= m_max; }

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader&) override;

    size_t m_value = 0;
};

ESSA_ENUM_TO_STRING(Progressbar::Labelling, __ENUMERATE_LABELLINGS)
ESSA_ENUM_FROM_STRING(Progressbar::Labelling, labelling, __ENUMERATE_LABELLINGS)

}
