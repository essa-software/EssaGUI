#include "Texture.hpp"

#include "Error.hpp"

#include <GL/glew.h>

#include <GL/gl.h>
#include <cassert>
#include <iostream>

namespace llgl::opengl {

Texture& Texture::operator=(Texture&& other)
{
    if (this == &other)
        return *this;
    if (m_id != 0) {
        // std::cout << "glDeleteTextures(" << m_id << ") in move=" << std::endl;
        glDeleteTextures(1, &m_id);
    }
    m_id = std::exchange(other.m_id, 0);
    m_size = std::exchange(other.m_size, {});
    return *this;
}

Texture::~Texture()
{
    if (m_id != 0) {
        // std::cout << "glDeleteTextures(" << m_id << ")" << std::endl;
        glDeleteTextures(1, &m_id);
    }
}

Texture Texture::create_from_image(Image const& image)
{
    return create_from_color_array(image.size(), image.pixels());
}

template<class T>
Texture Texture::create_from_color_array(Util::Vector2u size, std::span<T const> array, Format format)
{
    Texture texture = create_empty(size);
    texture.update({}, size, array, format);
    return texture;
}

template Texture Texture::create_from_color_array(Util::Vector2u size, std::span<Util::Color const> array, Format format);
template Texture Texture::create_from_color_array(Util::Vector2u size, std::span<Util::Colorf const> array, Format format);

Texture Texture::create_empty(Util::Vector2u size, Format format)
{
    Texture texture;
    texture.m_size = size;
    // std::cout << "glGenTextures() = " << texture.m_id << std::endl;
    texture.recreate(size, format);
    return texture;
}

static auto gl_format(Texture::Format format)
{
    switch (format) {
        case Texture::Format::RGBA:
            return GL_RGBA;
        case Texture::Format::RGB:
            return GL_RGB;
    }
    return 0;
}

void Texture::ensure_initialized(Format format)
{
    if (m_id == 0) {
        glGenTextures(1, &m_id);

        std::cout << "Ensure initialized " << m_id << std::endl;
        set_filtering(Filtering::Nearest);

        TextureBinder binder(*this);
        // std::cout << "glTexImage2D(" << m_size << ")\n";
        glTexImage2D(GL_TEXTURE_2D, 0, gl_format(format), m_size.x(), m_size.y(), 0, gl_format(format), GL_FLOAT, nullptr);
    }
}

void Texture::recreate(Util::Vector2u size, Format format)
{
    m_size = size;

    if (m_id) {
        TextureBinder binder(*this);
        // std::cout << "glTexImage2D(" << m_size << ")\n";
        glTexImage2D(GL_TEXTURE_2D, 0, gl_format(format), m_size.x(), m_size.y(), 0, gl_format(format), GL_FLOAT, nullptr);
    }

    ensure_initialized(format);
}

template<class PixelType>
static void update_part_of_bound_texture(Util::Vector2u position, Util::Vector2u size, std::span<PixelType const> pixels, Texture::Format format)
{
    auto type = [&]() {
        if constexpr (std::is_same_v<PixelType, Util::Colorf>)
            return GL_FLOAT;
        if constexpr (std::is_same_v<PixelType, Util::Color>)
            return GL_UNSIGNED_BYTE;
        __builtin_unreachable();
    }();

    // std::cout << "glTexSubImage2D(" << position << "," << size << ")\n";
    glTexSubImage2D(GL_TEXTURE_2D, 0, position.x(), position.y(), size.x(), size.y(), gl_format(format), type, pixels.data());
}

template<class T>
void Texture::update(Util::Vector2u dst_position, Util::Vector2u src_size, std::span<T const> pixels, Format format)
{
    assert(dst_position.x() + src_size.x() <= m_size.x());
    assert(dst_position.y() + src_size.y() <= m_size.y());
    // TODO: Support RGB format
    assert(format == Texture::Format::RGBA && pixels.size() == src_size.x() * src_size.y());

    ensure_initialized(format);
    TextureBinder binder(*this);
    update_part_of_bound_texture(dst_position, src_size, pixels, format);
}

template void Texture::update(Util::Vector2u dst_position, Util::Vector2u src_size, std::span<Util::Color const> pixels, Format format);
template void Texture::update(Util::Vector2u dst_position, Util::Vector2u src_size, std::span<Util::Colorf const> pixels, Format format);

void Texture::set_filtering(Filtering filtering)
{
    TextureBinder binder(*this);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering == Filtering::Nearest ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering == Filtering::Nearest ? GL_NEAREST : GL_LINEAR);
}

void Texture::bind(Texture const* texture)
{
    glBindTexture(GL_TEXTURE_2D, texture ? texture->id() : 0);
}

void Texture::set_label(std::string const& label)
{
    glObjectLabel(GL_TEXTURE, m_id, label.size(), label.data());
}

}
