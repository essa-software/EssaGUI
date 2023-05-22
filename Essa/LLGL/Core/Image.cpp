#include "Image.hpp"

#include <cassert>
#include <utility>

namespace llgl {

Image::Image(Image&& other) {
    m_pixels = std::exchange(other.m_pixels, nullptr);
    m_size = std::exchange(other.m_size, {});
}

Image& Image::operator=(Image&& other) {
    if (this == &other)
        return *this;

    this->~Image();
    m_pixels = std::exchange(other.m_pixels, nullptr);
    m_size = std::exchange(other.m_size, {});

    return *this;
}

Image Image::create_uninitialized(Util::Size2u size) {
    Image image { size };
    return image;
}

Image Image::create_filled_with_color(Util::Size2u size, Util::Color color) {
    Image image { size };
    std::fill(image.m_pixels, image.m_pixels + image.pixel_count(), color);
    return image;
}

void Image::insert_image_in_bounds(Image const& other, Util::Point2u where) {
    assert(where.x() + other.size().x() <= m_size.x());
    assert(where.y() + other.size().y() <= m_size.y());
    for (unsigned x = 0; x < other.size().x(); x++) {
        for (unsigned y = 0; y < other.size().y(); y++) {
            set_pixel({ x + where.x(), y + where.y() }, other.get_pixel({ x, y }));
        }
    }
}

bool Image::is_point_in_bounds(Util::Point2f const& point) const {
    return point.x() >= 0 && point.x() < m_size.x() && point.y() >= 0 && point.y() < m_size.y();
}

Image::Image(Util::Size2u size) {
    m_size = size;
    m_pixels = new Util::Color[pixel_count()];
}

Image::~Image() {
    delete[] m_pixels;
}

}
