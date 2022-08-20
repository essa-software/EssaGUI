#pragma once

#include "DrawState.hpp"

#include <EssaUtil/DelayedInit.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/Projection.hpp>

namespace llgl
{

class StateScope
{
public:
    StateScope(DrawState const&, Projection const&);

    opengl::ShaderScope& shader_scope() { return m_shader_scope; }

private:
    opengl::ShaderScope m_shader_scope;
    Util::DelayedInit<opengl::TextureBinder> m_texture_binder;
};

}
