#pragma once

#include <EssaGUI/util/ConfigFile.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/Resources/TTFFont.hpp>
#include <compare>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace Gfx {

template<class T>
struct ResourceTraits;

template<class T>
concept Resource = requires() {
    { ResourceTraits<T>::load_from_file("") } -> std::convertible_to<std::optional<T>>;
    { ResourceTraits<T>::base_path() } -> std::convertible_to<std::string_view>;
};

using Font = llgl::TTFFont;
using Texture = llgl::Texture;

template<>
struct ResourceTraits<Font> {
    static std::optional<Font> load_from_file(std::string const&);
    static std::string_view base_path() { return "fonts"; }
};

template<>
struct ResourceTraits<Texture> {
    static std::optional<Texture> load_from_file(std::string const&);
    static std::string_view base_path() { return "textures"; }
};

class ResourceId {
public:
    enum class Type {
        External, // Uses system path resolution
        Asset     // Uses predefined directories and resource-dependend base paths
    };

    static ResourceId external(std::string const& path) {
        return ResourceId { Type::External, path };
    }

    static ResourceId asset(std::string const& path) {
        return ResourceId { Type::Asset, path };
    }

    Type type() const { return m_type; }
    std::string path() const { return m_path; }

    bool operator==(ResourceId const&) const = default;
    std::strong_ordering operator<=>(ResourceId const&) const = default;

    friend std::ostream& operator<<(std::ostream& out, ResourceId const& id) {
        return out << (id.m_type == Type::Asset ? "asset:" : "external:") << id.m_path;
    }

private:
    ResourceId(Type type, std::string path)
        : m_type(type)
        , m_path(std::move(path)) { }

    Type m_type {};
    std::string m_path;
};

namespace Detail {
struct ResourceIdAndBase {
    ResourceId id;
    std::string_view base;

    bool operator==(ResourceIdAndBase const&) const = default;
    std::strong_ordering operator<=>(ResourceIdAndBase const&) const = default;

    friend std::ostream& operator<<(std::ostream& out, ResourceIdAndBase const& id_and_base) {
        return out << "[" << id_and_base.id << "@" << id_and_base.base << "]";
    }
};

struct ResourceWrapperBase {
    virtual ~ResourceWrapperBase() = default;
};

template<Resource T>
struct ResourceWrapper : public ResourceWrapperBase {
    ResourceWrapper(T&& res)
        : resource(std::forward<T>(res)) { }
    T resource;
};

}

class ResourceManager {
public:
    ResourceManager();

    Util::ConfigFile const& config() const { return m_config; }

    std::optional<std::string> lookup_resource(ResourceId, std::string_view base) const;
    std::string require_lookup_resource(ResourceId, std::string_view base) const;

    template<Resource T>
    T* get(ResourceId const& id) const {
        using Traits = ResourceTraits<T>;
        Detail::ResourceIdAndBase id_and_base { id, Traits::base_path() };

        auto cached_resource = m_cached_resources.find(id_and_base);
        if (cached_resource != m_cached_resources.end()) {
            auto ptr = cached_resource->second.get();
            if (!ptr)
                return nullptr;
            return &static_cast<Detail::ResourceWrapper<T>*>(cached_resource->second.get())->resource;
        }

        auto fs_path = lookup_resource(id_and_base.id, id_and_base.base);
        if (!fs_path) {
            std::cout << "ResourceManager: Failed to lookup resource (" << typeid(T).name() << ") '" << id_and_base << "'" << std::endl;
            return nullptr;
        }

        using Traits = ResourceTraits<T>;
        auto resource = Traits::load_from_file(*fs_path);
        if (!resource) {
            m_cached_resources.insert({ id_and_base, nullptr });
            std::cout << "ResourceManager: Failed to load resource (" << typeid(T).name() << ") '" << id_and_base << "' from " << *fs_path << std::endl;
            return nullptr;
        }
        return &static_cast<Detail::ResourceWrapper<T>*>(
            m_cached_resources.emplace(std::piecewise_construct, std::make_tuple(id_and_base), std::make_tuple(std::make_unique<Detail::ResourceWrapper<T>>(std::forward<T>(*resource)))).first->second.get())
                    ->resource;
    }

    template<Resource T>
    T& require(ResourceId const& path) const {
        auto resource = get<T>(path);
        if (!resource) {
            std::cout << "ResourceManager: Aborting because resource '" << path << "' is required to run" << std::endl;
            exit(1);
        }
        return *resource;
    }

    template<Resource T>
    T& require(std::string path) const {
        return require<T>(ResourceId::asset(std::move(path)));
    }

    template<Resource T>
    T& require_external(std::string path) const {
        return require<T>(ResourceId::external(std::move(path)));
    }

    Texture& require_texture(std::string path) const {
        return require<Texture>(std::move(path));
    }
    Font& require_font(std::string path) const {
        return require<Font>(std::move(path));
    }

    llgl::TTFFont& font() const;
    llgl::TTFFont& bold_font() const;
    llgl::TTFFont& fixed_width_font() const;

private:
    void find_resource_roots();

    std::vector<std::string> m_resource_roots;
    Util::ConfigFile m_config;

    mutable std::map<Detail::ResourceIdAndBase, std::unique_ptr<Detail::ResourceWrapperBase>> m_cached_resources;
};

}
