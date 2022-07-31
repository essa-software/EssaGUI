#pragma once

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/Vector.hpp>
#include <span>

namespace llgl {

class Image {
public:
    Image(Image const&) = delete;
    Image& operator=(Image const&) = delete;
    Image(Image&&);
    Image& operator=(Image&&);
    ~Image();

    static Image create_uninitialized(Util::Vector2u size);
    static Image create_filled_with_color(Util::Vector2u size, Util::Color color);

    std::span<Util::Color const> pixels() const { return { m_pixels, pixel_count() }; }
    std::span<Util::Color> pixels() { return { m_pixels, pixel_count() }; }

    ESSA_ALWAYS_INLINE Util::Color get_pixel(Util::Vector2u pos) const { return m_pixels[pixel_index(pos)]; }
    ESSA_ALWAYS_INLINE void set_pixel(Util::Vector2u pos, Util::Color color) { m_pixels[pixel_index(pos)] = color; }

    void insert_image_in_bounds(Image const& other, Util::Vector2u where);

    auto size() const { return m_size; }
    size_t pixel_count() const { return m_size.x() * m_size.y(); }

private:
    Image(Util::Vector2u size);

    ESSA_ALWAYS_INLINE size_t pixel_index(Util::Vector2u pos) const { return pos.x() + pos.y() * m_size.x(); }

    Util::Color* m_pixels = nullptr;
    Util::Vector2u m_size;
};

}
