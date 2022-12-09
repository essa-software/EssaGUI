#pragma once

#include "DefaultGUIShader.hpp"
#include <Essa/GUI/TextAlign.hpp>
#include <Essa/LLGL/OpenGL/PrimitiveType.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <span>

namespace GUI {

class Window : public llgl::Window {
public:
    Window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& = {});
    virtual ~Window() = default;

    void clear(Util::Color = Util::Colors::Black);
};

}
