#include "EMLResource.hpp"

#include <EssaUtil/Stream/File.hpp>
#include <EssaUtil/Stream/Reader.hpp>

std::optional<EML::EMLResource> Gfx::ResourceTraits<EML::EMLResource>::load_from_file(std::string const& path) {
    auto maybe_file = Util::ReadableFileStream::open(path);
    if (maybe_file.is_error())
        return {};

    auto file = maybe_file.release_value();

    auto maybe_buffer = Util::Reader { file }.read_while([](uint8_t) { return true; });
    if (maybe_buffer.is_error())
        return {};

    auto buffer = maybe_buffer.release_value();
    return EML::EMLResource { { reinterpret_cast<char const*>(buffer.begin()), buffer.size() } };
}
