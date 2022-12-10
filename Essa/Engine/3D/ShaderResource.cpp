#include "ShaderResource.hpp"
#include <EssaUtil/Stream/File.hpp>
#include <EssaUtil/Stream/Reader.hpp>

Util::OsErrorOr<std::string> Essa::Detail::read_file(std::string const& filename) {
    auto stream = TRY(Util::ReadableFileStream::open(filename));
    Util::TextReader reader { stream };
    auto v = TRY(reader.consume_while([](auto) { return true; }));
    return v.encode();
}
