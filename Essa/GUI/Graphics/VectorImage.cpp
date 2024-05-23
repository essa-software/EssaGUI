#include "VectorImage.hpp"

#include <EssaUtil/Stream/File.hpp>

#include <cstddef>
extern "C" {
#include <tinyvg.h>
}

namespace Gfx {

llgl::Image TVGVectorImage::render(Util::Size2u size) const {
    tinyvg_Bitmap bitmap;
    auto err = tinyvg_render_bitmap(m_tvg_data.begin(), m_tvg_data.size(), TINYVG_AA_x64, size.x(), size.y(), &bitmap);
    if (err != TINYVG_SUCCESS) {
        fmt::print("Failed to render TVG image: {}\n", (int)err);
        return llgl::Image::create_uninitialized({});
    }
    auto image = llgl::Image::create_uninitialized({ bitmap.width, bitmap.height });
    std::memcpy(image.pixels().data(), bitmap.pixels, static_cast<size_t>(bitmap.width) * bitmap.height * 4);
    tinyvg_free_bitmap(&bitmap);
    return image;
}

std::optional<TVGVectorImage> TVGVectorImage::load_from_file(std::string const& path) {
    auto maybe_buffer = Util::ReadableFileStream::read_file(path);
    if (maybe_buffer.is_error()) {
        return std::nullopt;
    }
    return TVGVectorImage(maybe_buffer.release_value());
}

}
