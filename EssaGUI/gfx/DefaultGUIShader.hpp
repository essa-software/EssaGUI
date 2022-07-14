#pragma once

#include <LLGL/OpenGL/Shader.hpp>

namespace Gfx {

llgl::opengl::Program& default_gui_shader();

class DefaultGUIShader : public llgl::opengl::Shader {
public:
    DefaultGUIShader()
        : llgl::opengl::Shader(Gfx::default_gui_shader()) { }

    virtual llgl::opengl::AttributeMapping attribute_mapping() const override { return { 1, 2, 3, 0 }; }

private:
    virtual void on_bind(llgl::opengl::ShaderScope&) const override;
};

}
