#pragma once

#include "EMLError.hpp"

#include <EssaUtil/GenericParser.hpp>
#include <EssaUtil/Stream/Stream.hpp>
#include <string>

namespace EML {

class Loader;
struct Object;

// A GUI object (Widget etc.) which can be loaded from EML
class EMLObject {
public:
    virtual ~EMLObject() = default;

    virtual EMLErrorOr<void> load_from_eml_object(Object const&, Loader& loader) = 0;

    [[nodiscard]] bool load_from_eml_string(std::string_view source);
    [[nodiscard]] bool load_from_eml_file(std::string const& path);
    [[nodiscard]] bool load_from_eml_stream(Util::ReadableStream&);
};

}
