#pragma once

#include <Essa/GUI/Graphics/Drawing/Shape.hpp>

namespace Gfx::Drawing {

class Ellipse : public Shape {
public:
    Ellipse(Util::Point2f center, float radius, Fill fill, Outline outline = Outline::none())
        : Shape(fill, outline)
        , m_extents(radius, radius) {
        move(center.to_vector());
    }

    Ellipse(Util::Point2f center, Util::Vector2f extents, Fill fill, Outline outline = Outline::none())
        : Shape(fill, outline)
        , m_extents(extents) {
        move(center.to_vector());
    }

    __ESSA_DEFINE_SHAPE_CHAINABLES(Ellipse)

    Ellipse& set_point_count(size_t s) {
        m_point_count = s;
        return *this;
    }

    Util::Vector2f extents() const {
        return m_extents;
    }
    virtual size_t point_count() const override {
        return m_point_count;
    }
    virtual Util::Point2f point(size_t idx) const override {
        assert(idx < m_point_count);
        float angle = static_cast<float>(idx * M_PI * 2) / m_point_count;
        return { std::sin(angle) * m_extents.x(), std::cos(angle) * m_extents.y() };
    }
    virtual Util::Rectf local_bounds() const override {
        return {
            -m_extents.to_point(),
            m_extents.to_size() * 2,
        };
    }

private:
    // FIXME: Is this size or vector?
    Util::Vector2f m_extents;
    size_t m_point_count = 16;
};

}
