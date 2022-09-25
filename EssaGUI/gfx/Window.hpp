#pragma once

#include "DefaultGUIShader.hpp"
#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <LLGL/OpenGL/PrimitiveType.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/VertexArray.hpp>
#include <LLGL/Resources/TTFFont.hpp>
#include <LLGL/Window/Window.hpp>
#include <span>

namespace GUI {

class Window : public llgl::Window {
public:
    Window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& = {});
    virtual ~Window() = default;

    void clear(Util::Color = Util::Colors::Black);
};

}
