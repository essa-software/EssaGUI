#pragma once

#include <LLGL/OpenGL/FBO.hpp>
#include <LLGL/Renderer/Renderer.hpp>

namespace llgl {

class RenderToTexture : public Renderer {
public:
    explicit RenderToTexture(Window const&);
    explicit RenderToTexture(Util::Vector2i size);

    virtual void clear(std::optional<Util::Color> color = {}) override;
    virtual void apply_projection(Projection const&) override;
    virtual Projection projection() const override;
    virtual void draw_vao(opengl::VAO const&, opengl::PrimitiveType, DrawState const&) override;
    opengl::Texture const& texture() const { return m_fbo.color_texture(); }
    void set_label(std::string const& label);

private:
    Window const* m_window = nullptr;
    opengl::FBO m_fbo;
    Projection m_projection;
};

}
