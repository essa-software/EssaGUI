#include "Renderer.hpp"

namespace llgl {

void Renderer::bind_if_not_bound(unsigned fbo) {
    static unsigned s_current_fbo = 0;
    if (s_current_fbo != fbo) {
        s_current_fbo = fbo;
        OpenGL::BindFramebuffer(GL_FRAMEBUFFER, fbo);
    }
}

}
