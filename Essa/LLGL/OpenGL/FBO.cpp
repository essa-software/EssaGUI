#include "FBO.hpp"

#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <Essa/AbstractOpenGLHelper.hpp>
#include <iostream>

namespace llgl::opengl {

FBO::FBO(Util::Size2u size) {
    OpenGL::GenFramebuffers(1, &m_fbo);

    resize(size);
    FBOScope scope { *this };
    OpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_renderbuffer);

    GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
    OpenGL::DrawBuffers(1, buffers);

    auto status = OpenGL::CheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO: Failed to create framebuffer (status=" << std::hex << status << std::dec << ")" << std::endl;
        return;
    }
}

static unsigned s_current_fbo = 0;

FBO::~FBO() {
    if (s_current_fbo == m_fbo) {
        s_current_fbo = 0;
    }
    OpenGL::DeleteFramebuffers(1, &m_fbo);
    if (m_depth_renderbuffer)
        OpenGL::DeleteRenderbuffers(1, &m_depth_renderbuffer);
}

void FBO::bind(Target target) const {
    s_current_fbo = m_fbo;
    OpenGL::BindFramebuffer(static_cast<GLenum>(target), m_fbo);
}

void FBO::resize(Util::Size2u size) {
    FBOScope scope { *this };
    if (m_color_texture.size() == size)
        return;
    if (!m_color_texture.id())
        m_color_texture = Texture::create_empty(size, Texture::Format::RGBA);
    else
        m_color_texture.recreate(size, Texture::Format::RGBA);
    OpenGL::FramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_color_texture.id(), 0);
    if (m_depth_renderbuffer)
        OpenGL::DeleteRenderbuffers(1, &m_depth_renderbuffer);
    OpenGL::GenRenderbuffers(1, &m_depth_renderbuffer);
    OpenGL::BindRenderbuffer(GL_RENDERBUFFER, m_depth_renderbuffer);
    OpenGL::RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x(), size.y());

    // std::cout << "FBO: recreated with size " << size.x << "," << size.y << std::endl;
}

void FBO::set_label(std::string const& str) {
    OpenGL::ObjectLabel(GL_FRAMEBUFFER, m_fbo, str.size(), str.data());
    m_color_texture.set_label("FBO Texture: " + str);
}

FBOScope::FBOScope(FBO const& fbo, FBO::Target target)
    : m_old_fbo(s_current_fbo)
    , m_target(target) {
    if (s_current_fbo == fbo.id())
        return;
    fbo.bind(m_target);
}

FBOScope::~FBOScope() {
    if (s_current_fbo == m_old_fbo)
        return;
    OpenGL::BindFramebuffer(static_cast<GLenum>(m_target), m_old_fbo);
    s_current_fbo = m_old_fbo;
}

}
