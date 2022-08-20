#pragma once

#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>

#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Matrix.hpp>
#include <LLGL/Core/Vertex.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <LLGL/OpenGL/VAO.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/Renderer/DrawState.hpp>
#include <LLGL/Resources/TTFFont.hpp>
#include <LLGL/Window/Window.hpp>
#include <span>

namespace GUI {

struct DrawOptions {
    Util::Color fill_color = Util::Colors::White;
    Util::Color outline_color = Util::Colors::White;
    llgl::opengl::Texture const* texture = nullptr;
    float outline_thickness = 0;
};

struct TextDrawOptions : public DrawOptions {
    int font_size = 30;
    Align text_align = Align::TopLeft;
};

struct RectangleDrawOptions : public DrawOptions {
    float border_radius_top_left = 0;
    float border_radius_top_right = 0;
    float border_radius_bottom_right = 0;
    float border_radius_bottom_left = 0;

    void set_border_radius(float br) {
        border_radius_top_left = border_radius_top_right = border_radius_bottom_right = border_radius_bottom_left = br;
    }
};

class Window : public llgl::Window {
public:
    Window(Util::Vector2i size, Util::UString const& title, llgl::ContextSettings const& = {});
    virtual ~Window() = default;

    void set_projection(llgl::Projection p) { m_projection = p; }
    llgl::Projection projection() const { return m_projection; }

    void set_texture(llgl::opengl::Texture const* tex) { m_texture = tex; }

    // If shader is nullptr, the default shader is used.
    // Uniforms set:
    // - texture : sampler2D     Texture to use, only if useTexture == true
    // - useTexture : bool       Whether texture is set
    // - projectionMatrix : mat4 The matrix from view()
    // - viewMatrix : mat4       The view matrix (from view_matrix())
    // - modelMatrix : mat4      The model matrix (from model_matrix())
    // Attributes:
    // 0 - position, 1 - color, 2 - tex coord
    void set_shader(llgl::opengl::Shader* shader) { m_shader = shader; }

    void set_model_matrix(Util::Matrix4x4f matrix) { m_model_matrix = matrix; }
    Util::Matrix4x4f model_matrix() const { return m_model_matrix; }
    void set_view_matrix(Util::Matrix4x4f matrix) { m_view_matrix = matrix; }
    Util::Matrix4x4f view_matrix() const { return m_view_matrix; }

    class ModelScope {
    public:
        ModelScope(Window& wnd, Util::Matrix4x4f mat)
            : m_window(wnd)
            , m_old_matrix(wnd.model_matrix()) {
            wnd.set_model_matrix(mat);
        }
        ~ModelScope() {
            m_window.set_model_matrix(m_old_matrix);
        }

    private:
        Window& m_window;
        Util::Matrix4x4f m_old_matrix;
    };

    void clear(Util::Color = Util::Colors::Black);

    void draw_vertices(llgl::opengl::PrimitiveType mode, std::span<llgl::Vertex const>);
    void draw_outline(std::span<Util::Vector3f const>, Util::Color color, float thickness);
    void draw_indexed_vertices(llgl::opengl::PrimitiveType mode, std::span<llgl::Vertex const>, std::span<unsigned const> indices);

    void draw_rectangle(Util::Rectf bounds, RectangleDrawOptions const& = {});
    void draw_text(Util::UString const&, llgl::TTFFont const&, Util::Vector2f position, TextDrawOptions const& = {});
    void draw_text_aligned_in_rect(Util::UString const&, Util::Rectf rect, llgl::TTFFont const&, TextDrawOptions const& = {});
    void draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& = {});

    // FIXME: Add some class like sf::Text.
    Util::Vector2u calculate_text_size(Util::UString const&, llgl::TTFFont const&, TextDrawOptions const& = {});
    float find_character_position(size_t index, Util::UString const&, llgl::TTFFont const&, TextDrawOptions const& = {});

private:
    void apply_states();
    llgl::DrawState draw_state() const;

    llgl::opengl::VAO m_temporary_vao;

    llgl::Projection m_projection;
    Util::Matrix4x4f m_model_matrix;
    Util::Matrix4x4f m_view_matrix;
    llgl::opengl::Shader* m_shader = nullptr;
    llgl::opengl::Texture const* m_texture = nullptr;
};

}
