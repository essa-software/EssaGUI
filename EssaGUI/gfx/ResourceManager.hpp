#pragma once

#include "EssaGUI/util/ConfigFile.hpp"
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/Resources/TTFFont.hpp>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Gfx {

template<class T>
struct ResourceTraits;

template<class T>
concept Resource = requires() {
    { ResourceTraits<T>::load_from_file("") } -> std::convertible_to<std::optional<T>>;
    { ResourceTraits<T>::base_path() } -> std::convertible_to<char const*>;
};

using Font = llgl::TTFFont;
using Texture = llgl::opengl::Texture;

template<>
struct ResourceTraits<Font> {
    static std::optional<Font> load_from_file(std::string const&);
    static char const* base_path() { return "fonts"; }
};

template<>
struct ResourceTraits<Texture> {
    static std::optional<Texture> load_from_file(std::string const&);
    static char const* base_path() { return "textures"; }
};

struct ResourceWrapperBase {
    virtual ~ResourceWrapperBase() = default;
};

template<class T>
struct ResourceWrapper : public ResourceWrapperBase {
    ResourceWrapper(T&& res)
        : resource(std::forward<T>(res)) { }
    T resource;
};

class ResourceManager {
public:
    ResourceManager();

    Util::ConfigFile const& config() const { return m_config; }

    std::optional<std::string> lookup_resource(std::string_view base, std::string const& path) const;
    std::string require_lookup_resource(std::string_view base, std::string const& path) const;

    template<class T>
    T* load(std::string const& path) const {
        using Traits = ResourceTraits<T>;
        auto resource = Traits::load_from_file(require_lookup_resource(Traits::base_path(), path));
        if (!resource) {
            m_cached_resources.insert({ path, nullptr });
            std::cout << "ResourceManager: Failed to load resource (" << typeid(T).name() << ") '" << path << "'" << std::endl;
            return nullptr;
        }
        return static_cast<ResourceWrapper<T>*>(&m_cached_resources.insert({ path, std::make_unique<ResourceWrapper<T>>(resource) }).first->second)->resource;
    }

    template<class T>
    T* get(std::string const& path) const {
        auto cached_resource = m_cached_resources.find(path);
        if (cached_resource != m_cached_resources.end()) {
            auto ptr = cached_resource->second.get();
            if (!ptr)
                return nullptr;
            return &static_cast<ResourceWrapper<T>*>(cached_resource->second.get())->resource;
        }

        using Traits = ResourceTraits<T>;

        auto resource = Traits::load_from_file(require_lookup_resource(Traits::base_path(), path));
        if (!resource) {
            m_cached_resources.insert({ path, nullptr });
            std::cout << "ResourceManager: Failed to load resource (" << typeid(T).name() << ") '" << path << "'" << std::endl;
            return nullptr;
        }
        return &static_cast<ResourceWrapper<T>*>(m_cached_resources.insert({ path, std::make_unique<ResourceWrapper<T>>(std::move(*resource)) }).first->second.get())->resource;
    }

    template<class T>
    T& require(std::string const& path) const {
        auto resource = get<T>(path);
        if (!resource) {
            std::cout << "ResourceManager: Aborting because the resource is required to run" << std::endl;
            exit(1);
        }
        return *resource;
    }

    Texture& require_texture(std::string const& path) const {
        return require<Texture>(path);
    }
    Font& require_font(std::string const& path) const {
        return require<Font>(path);
    }

    llgl::TTFFont& font() const;
    llgl::TTFFont& bold_font() const;
    llgl::TTFFont& fixed_width_font() const;

private:
    void find_resource_roots();

    std::vector<std::string> m_resource_roots;
    Util::ConfigFile m_config;
    mutable std::map<std::string, std::unique_ptr<ResourceWrapperBase>> m_cached_resources;
};

}
