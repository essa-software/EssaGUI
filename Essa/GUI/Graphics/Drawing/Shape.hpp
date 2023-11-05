#pragma once

#include <Essa/GUI/Graphics/DefaultGUIShader.hpp>
#include <Essa/GUI/Graphics/Drawing/Fill.hpp>
#include <Essa/GUI/Graphics/Drawing/Outline.hpp>
#include <Essa/GUI/Graphics/GUIBuilder.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/DynamicShader.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
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
    template<llgl::ShaderImplPartial S> Shape& set_shader(S& shader, typename S::Uniforms uniforms) {
        m_shader_context = Gfx::ShaderContext { llgl::DynamicShader<Gfx::Vertex>(shader), std::move(uniforms) };
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
    std::optional<ShaderContext> shader_context() const { return m_shader_context; }

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
            : m_shape(s) { }

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

    std::vector<Util::Point2f> const& vertices() const;

protected:
    void invalidate() { m_vertex_cache_dirty = true; }

private:
    llgl::Transform m_transform;
    Util::Point2f m_origin;
    Fill m_fill;
    Outline m_outline;
    std::optional<ShaderContext> m_shader_context;

    mutable bool m_vertex_cache_dirty = true;
    mutable std::vector<Util::Point2f> m_vertex_cache;
};

struct RoundingResult {
    Util::Point2f center;
    float angle_start;
    float angle_end;
    float scaled_radius;
};
struct RoundingSettings {
    Util::Point2f left;
    Util::Point2f right;
    Util::Point2f tip;
    float radius;
};

RoundingResult round(RoundingSettings settings);

}

#define __ESSA_DEFINE_SHAPE_CHAINABLES(Subclass)                                                           \
    Subclass& set_transform(llgl::Transform t) { return static_cast<Subclass&>(Shape::set_transform(t)); } \
    Subclass& set_origin(Util::Point2f t) { return static_cast<Subclass&>(Shape::set_origin(t)); }         \
    template<llgl::ShaderImplPartial S> Subclass& set_shader(S& shader, typename S::Uniforms uniforms) {   \
        return static_cast<Subclass&>(Shape::set_shader(shader, std::move(uniforms)));                     \
    }                                                                                                      \
    Subclass& move(Util::Vector2f const& t) { return static_cast<Subclass&>(Shape::move(t)); }             \
    Subclass& rotate(float t) { return static_cast<Subclass&>(Shape::rotate(t)); }                         \
    Subclass& scale(Util::Vector2f const& t) { return static_cast<Subclass&>(Shape::scale(t)); }
