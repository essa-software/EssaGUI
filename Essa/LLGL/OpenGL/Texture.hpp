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
    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

    Texture(Texture&& other) {
        *this = std::move(other);
    }

    Texture& operator=(Texture&& other);

    // TODO: createFromImage
    // TODO: Add some Image/Bitmap class
    enum class Format {
        RGBA,
        RGB
    };

    static Texture create_from_image(Image const&);

    template<class T>
    static Texture create_from_color_array(Util::Cs::Size2u, std::span<T const> array, Format = Format::RGBA);

    static Texture create_empty(Util::Cs::Size2u, Format = Format::RGBA);

    Image copy_to_image() const;

    template<class T>
    void update(Util::Cs::Point2u dst_pos, Util::Cs::Size2u src_size, std::span<T const> array, Format format);

    void recreate(Util::Cs::Size2u, Format);

    enum class Filtering {
        Nearest,
        Linear
        // TODO: Mipmap filtering
    };

    void set_filtering(Filtering);

    unsigned id() const { return m_id; }
    Util::Cs::Size2u size() const { return m_size; }

    void set_label(std::string const&);

    static void bind(Texture const* texture);

private:
    void ensure_initialized(Format);

    unsigned m_id { 0 };
    Util::Cs::Size2u m_size;
};

class TextureBinder {
public:
    TextureBinder(Texture const& texture) { Texture::bind(&texture); }
    ~TextureBinder() { Texture::bind(nullptr); }
};

}
