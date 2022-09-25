#pragma once

#include <GL/glew.h>

#include <EssaUtil/Vector.hpp>

#include <LLGL/OpenGL/Texture.hpp>
#include <string>

namespace llgl::opengl {

class FBO {
public:
    explicit FBO(Util::Vector2u size);
    ~FBO();

    enum class Target {
        DrawFramebuffer = GL_DRAW_FRAMEBUFFER,
        ReadFramebuffer = GL_READ_FRAMEBUFFER,
        Framebuffer = GL_FRAMEBUFFER,
    };

    void bind(Target = Target::Framebuffer) const;
    void resize(Util::Vector2u);
    Texture const& color_texture() const { return m_color_texture; }
    unsigned id() const { return m_fbo; }

    void set_label(std::string const&);

private:
    unsigned m_fbo = 0;
    Texture m_color_texture;
    unsigned m_depth_renderbuffer = 0;
};

class FBOScope {
public:
    FBOScope(FBO const& fbo, FBO::Target target = FBO::Target::Framebuffer);
    ~FBOScope();

private:
    unsigned m_old_fbo = 0;
    FBO::Target m_target {};
};

}
