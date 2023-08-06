#pragma once

#include <Essa/LLGL/Core/Image.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Rect.hpp>
#include <span>
#include <string>
#include <utility>

namespace llgl {

class Texture {
public:
    Texture() = default;
    ~Texture();
    Texture(Texture const& other) = delete;
    Texture& operator=(Texture const& other) = delete;

    Texture(Texture&& other) { *this = std::move(other); }

    Texture& operator=(Texture&& other);

    // TODO: createFromImage
    // TODO: Add some Image/Bitmap class
    enum class Format { RGBA, RGB };

    static Texture create_from_image(Image const&);

    template<class T> static Texture create_from_color_array(Util::Size2u, std::span<T const> array, Format = Format::RGBA);

    static Texture create_empty(Util::Size2u, Format = Format::RGBA);

    Image copy_to_image() const;

    template<class T> void update(Util::Point2u dst_pos, Util::Size2u src_size, std::span<T const> array, Format format);

    void recreate(Util::Size2u, Format);

    enum class Filtering {
        Nearest,
        Linear
        // TODO: Mipmap filtering
    };

    void set_filtering(Filtering);

    unsigned id() const { return m_id; }
    Util::Size2u size() const { return m_size; }

    void set_label(std::string const&);

    static void bind(Texture const* texture);

private:
    void ensure_initialized(Format);

    unsigned m_id { 0 };
    Util::Size2u m_size;
};

class TextureBinder {
public:
    TextureBinder(Texture const& texture) { Texture::bind(&texture); }
    ~TextureBinder() { Texture::bind(nullptr); }
};

}
