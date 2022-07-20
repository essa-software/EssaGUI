#pragma once

#include "EssaGUI/util/ConfigFile.hpp"
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/Resources/TTFFont.hpp>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace Gfx {

class ResourceManager {
public:
    ResourceManager();

    enum class ResourceType {
        Texture, // /textures
        Font,    // /fonts
        Data,    // /data
        Generic, // /
    };

    Util::ConfigFile const& config() const { return m_config; }

    std::optional<std::string> lookup_resource(std::string const& path, ResourceType) const;
    std::string require_lookup_resource(std::string const& path, ResourceType) const;

    llgl::opengl::Texture& require_texture(std::string const& path) const;
    llgl::TTFFont& require_font(std::string const& path) const;

    llgl::TTFFont& font() const;
    llgl::TTFFont& bold_font() const;
    llgl::TTFFont& fixed_width_font() const;

private:
    void find_resource_roots();

    std::vector<std::string> m_resource_roots;
    Util::ConfigFile m_config;
    mutable std::map<std::string, llgl::opengl::Texture> m_cached_textures;
    mutable std::map<std::string, llgl::TTFFont> m_cached_fonts;
};

}
