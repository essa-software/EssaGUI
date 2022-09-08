#pragma once

#include <EssaGUI/gfx/ResourceManager.hpp>

namespace EML {

// ResourceManager-compatible wrapper for EML
struct EMLResource {
    std::string source;
};

}

template<>
struct Gfx::ResourceTraits<EML::EMLResource> {
    static std::optional<EML::EMLResource> load_from_file(std::string const& path);
    static std::string_view base_path() { return "layouts"; }
};
