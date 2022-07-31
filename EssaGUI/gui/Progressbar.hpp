#pragma once

#include "Widget.hpp"

#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <functional>

namespace GUI {

class Progressbar : public Widget {
public:
    explicit Progressbar(Container& parent, size_t min, size_t max)
        : Widget(parent)
        , m_min(min)
        , m_max(max) { }

    virtual void draw(GUI::Window& window) const override;

    enum class Labelling {
        PERCENTAGE,
        VALUE,
        NONE
    };

    Util::UString get_content() const { return m_content; }
    void set_content(Util::UString const& content) { m_content = content; }

    Util::UString get_content_with_labelling() const;
    void step_by();

    size_t get_value() const { return m_value; }
    void set_value(size_t const& val) {
        m_value = val;
        if (m_value >= m_max && on_finish)
            on_finish();
    }

    Labelling get_labelling() const { return m_labelling; }
    void set_labelling(Labelling const& labelling) { m_labelling = labelling; }

    size_t get_min() const { return m_min; }
    void set_min(size_t const& min) { m_min = min; }

    size_t get_max() const { return m_max; }
    void set_max(size_t const& max) { m_max = max; }

    size_t get_step() const { return m_step; }
    void set_step(size_t const& step) { m_step = step; }

    Util::Color get_progressbar_color() const { return m_processbar_color; }
    void set_progressbar_color(Util::Color color) { m_processbar_color = color; }

    std::function<void()> on_finish;

    bool finished() const { return m_value >= m_max; }

private:
    size_t m_min = 0, m_max = 1;
    Util::Color m_processbar_color = Util::Colors::green;
    size_t m_value = 0, m_step = 1;
    Util::UString m_content = "";

    Labelling m_labelling = Labelling::NONE;
};

}
