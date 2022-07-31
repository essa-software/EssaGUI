#pragma once

#include "DrawState.hpp"

#include <EssaUtil/DelayedInit.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/View.hpp>

namespace llgl
{

class StateScope
{
public:
    StateScope(DrawState const&, View const&);

    opengl::ShaderScope& shader_scope() { return m_shader_scope; }

private:
    opengl::ShaderScope m_shader_scope;
    Util::DelayedInit<opengl::TextureBinder> m_texture_binder;
};

}
