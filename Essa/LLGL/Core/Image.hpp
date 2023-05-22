#pragma once

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/CoordinateSystem.hpp>
#include <EssaUtil/Vector.hpp>
#include <cstddef>
#include <span>

namespace llgl {

class Image {
public:
    Image(Image const&) = delete;
    Image& operator=(Image const&) = delete;
    Image(Image&&);
    Image& operator=(Image&&);
    ~Image();

    static Image create_uninitialized(Util::Size2u);
    static Image create_filled_with_color(Util::Size2u, Util::Color color);

    std::span<Util::Color const> pixels() const { return { m_pixels, pixel_count() }; }
    std::span<Util::Color> pixels() { return { m_pixels, pixel_count() }; }

    ESSA_ALWAYS_INLINE Util::Color get_pixel(Util::Point2u pos) const { return m_pixels[pixel_index(pos)]; }
    ESSA_ALWAYS_INLINE void set_pixel(Util::Point2u pos, Util::Color color) { m_pixels[pixel_index(pos)] = color; }

    void insert_image_in_bounds(Image const& other, Util::Point2u where);

    auto size() const { return m_size; }
    size_t pixel_count() const { return static_cast<size_t>(m_size.x()) * m_size.y(); }
    bool is_point_in_bounds(Util::Point2f const&) const;

private:
    Image(Util::Size2u size);

    ESSA_ALWAYS_INLINE size_t pixel_index(Util::Point2u pos) const { return pos.x() + pos.y() * m_size.x(); }

    Util::Color* m_pixels = nullptr;
    Util::Size2u m_size;
};

}
