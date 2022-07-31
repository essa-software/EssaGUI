#pragma once

#include "DrawState.hpp"
#include <LLGL/Core/Vertex.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/View.hpp>
#include <optional>
#include <span>

namespace llgl {

class Renderable;
class Window;

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void apply_view(View const&) = 0;
    virtual View view() const = 0;
    virtual void draw_vao(opengl::VAO const&, opengl::PrimitiveType, DrawState const&) = 0;

    virtual void clear(std::optional<Util::Color> color = {});
    void render_object(Renderable const&, DrawState = {});
};

}
