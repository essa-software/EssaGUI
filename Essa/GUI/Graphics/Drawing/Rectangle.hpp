#pragma once

#include <Essa/GUI/Graphics/Drawing/Shape.hpp>

namespace Gfx::Drawing {

class Rectangle : public Shape {
public:
    Rectangle(Util::Size2f size, Fill fill, Outline outline = Outline::none())
        : Shape(fill, outline)
        , m_size(size) { }

    Rectangle(Util::Rectf rect, Fill fill, Outline outline = Outline::none())
        : Shape(fill, outline)
        , m_size(rect.size()) {
        move({ rect.position().x(), rect.position().y() });
    }

    __ESSA_DEFINE_SHAPE_CHAINABLES(Rectangle)

    Util::Size2f size() const { return m_size; }

    virtual size_t point_count() const override { return 4; }
    virtual Util::Point2f point(size_t idx) const override {
        switch (idx) {
        case 0:
            return { 0, 0 };
        case 1:
            return { m_size.x(), 0 };
        case 2:
            return { m_size.x(), m_size.y() };
        case 3:
            return { 0, m_size.y() };
        }
        ESSA_UNREACHABLE;
    }
    virtual Util::Rectf local_bounds() const override { return { {}, m_size }; }

private:
    Util::Size2f m_size;
};

}
