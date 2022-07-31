#include "Image.hpp"

#include <cassert>
#include <utility>

namespace llgl {

Image::Image(Image&& other)
{
    m_pixels = std::exchange(other.m_pixels, nullptr);
    m_size = std::exchange(other.m_size, {});
}

Image& Image::operator=(Image&& other)
{
    if (this == &other)
        return *this;

    this->~Image();
    m_pixels = std::exchange(other.m_pixels, nullptr);
    m_size = std::exchange(other.m_size, {});

    return *this;
}

Image Image::create_uninitialized(Util::Vector2u size)
{
    Image image { size };
    return image;
}

Image Image::create_filled_with_color(Util::Vector2u size, Util::Color color)
{
    Image image { size };
    for (unsigned x = 0; x < image.size().x(); x++) {
        for (unsigned y = 0; y < image.size().y(); y++) {
            image.set_pixel({ x, y }, color);
        }
    }
    return image;
}

void Image::insert_image_in_bounds(Image const& other, Util::Vector2u where)
{
    assert(where.x() + other.size().x() <= m_size.x());
    assert(where.y() + other.size().y() <= m_size.y());
    for (unsigned x = 0; x < other.size().x(); x++) {
        for (unsigned y = 0; y < other.size().y(); y++) {
            set_pixel({ x + where.x(), y + where.y() }, other.get_pixel({ x, y }));
        }
    }
}

Image::Image(Util::Vector2u size)
{
    m_size = size;
    m_pixels = new Util::Color[pixel_count()];
}

Image::~Image()
{
    delete[] m_pixels;
}

}
