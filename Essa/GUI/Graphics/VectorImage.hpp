#pragma once

#include "ResourceManager.hpp"
#include <Essa/LLGL/Core/Image.hpp>
#include <EssaUtil/Buffer.hpp>

namespace Gfx {

class TVGVectorImage {
public:
    llgl::Image render(Util::Size2u size) const;

    static std::optional<TVGVectorImage> load_from_file(std::string const& path);

private:
    explicit TVGVectorImage(Util::Buffer data)
        : m_tvg_data(std::move(data)) { }

    Util::Buffer m_tvg_data;
};

template<>
struct ResourceTraits<TVGVectorImage> {
    static std::optional<TVGVectorImage> load_from_file(std::string const& path) {
        return TVGVectorImage::load_from_file(path);
    }
    static std::string_view base_path() {
        return "icons";
    }
};

}
