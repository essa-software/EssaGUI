#pragma once

#include <Essa/GUI/Graphics/ResourceManager.hpp>

namespace EML {

// ResourceManager-compatible wrapper for EML
struct EMLResource {
    std::string const& source_string() const {
        return m_source_string;
    }

private:
    friend Gfx::ResourceTraits<EML::EMLResource>;

    explicit EMLResource(std::string source_string)
        : m_source_string(std::move(source_string)) { }

    std::string m_source_string;
};

}

template<>
struct Gfx::ResourceTraits<EML::EMLResource> {
    static std::optional<EML::EMLResource> load_from_file(std::string const& path);
    static std::string_view base_path() {
        return "layouts";
    }
};
