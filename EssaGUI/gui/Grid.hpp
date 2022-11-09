#pragma once

#include <EssaGUI/gui/Border.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <vector>

namespace GUI {

class Grid : public Container {
public:
    friend Border;

    virtual void on_init() override {
        set_layout<VerticalBoxLayout>();
    }

    void set_spacing(float spacing);
    float spacing() const { return m_spacing; }

    void set_line_height(float height);
    float line_height() const { return m_line_height; }

    void resize(size_t width, size_t height);
    void set_width(size_t width);
    void set_height(size_t height);

    size_t width() const { return m_width; }
    size_t height() const { return m_height; }

    Border* get_cell(size_t x, size_t y);

private:
    float m_spacing = 0, m_line_height = 50;
    size_t m_width = 0, m_height = 0;

    std::vector<std::vector<Border*>> m_cells;

protected:
    using Container::add_widget;
    using Container::add_created_widget;
    using Container::clear_layout;
    using Container::get_layout;
    using Container::shrink;
    using Container::widgets;
};

}
