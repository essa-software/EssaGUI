#pragma once

#include <Essa/GUI/Graphics/Drawing/Fill.hpp>
#include <Essa/GUI/Graphics/Drawing/Outline.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <iterator>

namespace Gfx::Drawing {

class Shape {
public:
    virtual ~Shape() = default;

    Shape(Fill fill, Outline outline)
        : m_fill(fill)
        , m_outline(outline) { }

    Shape& set_transform(llgl::Transform t) {
        m_transform = t;
        return *this;
    }
    Shape& set_origin(Util::Point2f o) {
        m_origin = o;
        return *this;
    }
    Shape& move(Util::Vector2f const& t) {
        m_transform = m_transform.translate(Util::Vector3f { t, 0.f });
        return *this;
    }
    Shape& rotate(float t) {
        m_transform = m_transform.rotate_z(t);
        return *this;
    }
    Shape& scale(Util::Vector2f vec) {
        m_transform = m_transform.scale_x(vec.x()).scale_y(vec.y());
        return *this;
    }

    llgl::Transform transform() const { return m_transform; }
    // All points are offsetted by -origin() before transforming.
    Util::Point2f origin() const { return m_origin; }
    Fill fill() const { return m_fill; }
    Outline outline() const { return m_outline; }

    virtual size_t point_count() const = 0;
    virtual Util::Point2f point(size_t idx) const = 0;

    // Bounds that are used for calculating texture rect. That is,
    // point at [size()] will use bottom right corner of texture rect.
    virtual Util::Rectf local_bounds() const = 0;

    class Points {
        struct PointIterator {
            Shape const* shape = nullptr;
            size_t index;

            Util::Point2f operator*() const { return shape->point(index); }
            Util::Point2f operator->() const { return shape->point(index); }
            PointIterator& operator++() {
                index++;
                return *this;
            }
            bool operator==(PointIterator const&) const = default;
        };

    public:
        explicit Points(Shape const& s)
            : m_shape(s) {
        }

        PointIterator begin() const { return PointIterator { &m_shape, 0 }; }
        PointIterator end() const { return PointIterator { &m_shape, m_shape.point_count() }; }

        std::vector<Util::Point2f> to_vector() const {
            std::vector<Util::Point2f> vector;
            for (size_t s = 0; s < m_shape.point_count(); s++) {
                vector.push_back(m_shape.point(s));
            }
            return vector;
        }

    private:
        Shape const& m_shape;
        size_t m_index = 0;
    };

    Points points() const { return Points { *this }; }

private:
    llgl::Transform m_transform;
    Util::Point2f m_origin;
    Fill m_fill;
    Outline m_outline;
};

}

#define __ESSA_DEFINE_SHAPE_CHAINABLES(Subclass)                                                           \
    Subclass& set_transform(llgl::Transform t) { return static_cast<Subclass&>(Shape::set_transform(t)); } \
    Subclass& move(Util::Vector2f const& t) { return static_cast<Subclass&>(Shape::move(t)); }         \
    Subclass& rotate(float t) { return static_cast<Subclass&>(Shape::rotate(t)); }                         \
    Subclass& scale(Util::Vector2f const& t) { return static_cast<Subclass&>(Shape::scale(t)); }
