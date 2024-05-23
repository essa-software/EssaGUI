#pragma once

#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <memory>

namespace Gfx {

namespace Detail {

class IconImpl {
public:
    virtual ~IconImpl() = default;
    // Render the icon to a texture. Size is used as a hint, and
    // the actual size of the texture may differ.
    virtual llgl::Texture const& render(Util::Size2u size) = 0;
};

}

// An abstraction for bitmaps/vector images/... that can be rendered to
// a texture.
class Icon {
public:
    /*restricted(Gfx::ResourceTraits<T>)*/ explicit Icon(std::unique_ptr<Detail::IconImpl> impl)
        : m_impl(std::move(impl)) { }

    llgl::Texture const& render(Util::Size2u size) const {
        return m_impl->render(size);
    }

private:
    std::unique_ptr<Detail::IconImpl> m_impl;
};

}
