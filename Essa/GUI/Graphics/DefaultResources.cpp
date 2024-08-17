#include "DefaultResources.hpp"
#include "Essa/GUI/Graphics/Icon.hpp"

#include <Essa/GUI/Graphics/VectorImage.hpp>
#include <Essa/LLGL/Resources/ImageLoader.hpp>

namespace Gfx {

std::optional<Texture> ResourceTraits<Texture>::load_from_file(std::string const& path) {
    auto image = llgl::ImageLoader::load_from_file(path);
    if (!image)
        return {};
    return llgl::Texture::create_from_image(*image);
}

std::optional<Font> ResourceTraits<Font>::load_from_file(std::string const& path) {
    return llgl::TTFFont::open_from_file(path);
}

namespace IconImpl {

#ifdef ESSA_ENABLE_TINYVG

class TVG : public Detail::IconImpl {
public:
    explicit TVG(TVGVectorImage image)
        : m_image(std::move(image)) { }

    llgl::Texture const& render(Util::Size2u size) override {
        if (m_cached_texture && m_cached_texture->size() == size) {
            return *m_cached_texture;
        }
        auto img = m_image.render(size);
        m_cached_texture = llgl::Texture::create_from_image(img);
        return *m_cached_texture;
    }

private:
    TVGVectorImage m_image;
    std::optional<llgl::Texture> m_cached_texture;
};

#endif

class Image : public Detail::IconImpl {
public:
    explicit Image(llgl::Image image)
        : m_image(std::move(image)) { }

    llgl::Texture const& render(Util::Size2u) override {
        // Note: Ignore the size, just return the texture as is
        if (m_cached_texture) {
            return *m_cached_texture;
        }
        m_cached_texture = llgl::Texture::create_from_image(m_image);
        return *m_cached_texture;
    }

private:
    llgl::Image m_image;
    std::optional<llgl::Texture> m_cached_texture;
};

}

std::optional<Icon> ResourceTraits<Icon>::load_from_file(std::string const& path) {
    auto extension = path.substr(path.find_last_of('.') + 1);
#ifdef ESSA_ENABLE_TINYVG
    if (extension == "tvg") {
        auto maybe_image = TVGVectorImage::load_from_file(path);
        if (!maybe_image) {
            return {};
        }
        return Icon(std::make_unique<IconImpl::TVG>(std::move(*maybe_image)));
    }
#else
    if (extension == "tvg") {
        fmt::println(stderr, "TinyVG support is disabled, cannot load TVG images");
        return {};
    }
#endif
    auto maybe_image = llgl::ImageLoader::load_from_file(path);
    if (!maybe_image) {
        return {};
    }
    return Icon(std::make_unique<IconImpl::Image>(std::move(*maybe_image)));
}

}
