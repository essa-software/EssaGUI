#pragma once

#include <GL/glew.h>

#include <EssaGUI/glwrapper/Vertex.hpp>
#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaGUI/util/Matrix.hpp>
#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <span>

namespace GUI {

struct DrawOptions {
    sf::Color fill_color = sf::Color::White;
    sf::Color outline_color = sf::Color::White;
    sf::Texture const* texture = nullptr;
    float outline_thickness = 0;
};

struct TextDrawOptions : public DrawOptions {
    unsigned font_size = 30;
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

class SFMLWindow : public sf::Window {
public:
    SFMLWindow(sf::VideoMode, sf::String title, unsigned style = sf::Style::Default, sf::ContextSettings = sf::ContextSettings {});
    virtual ~SFMLWindow();

    // TODO: Support arbitrary view matrix
    void set_view(sf::View view);
    sf::View view() const { return m_view; }

    void set_texture(sf::Texture const* tex) { m_texture = tex; }

    // If shader is nullptr, the default shader is used.
    // Uniforms set:
    // - texture : sampler2D    Texture to use, only if useTexture == true
    // - useTexture : bool      Whether texture is set
    // - viewMatrix : mat4      The view matrix (from view() sf::View)
    // - modelMatrix : mat4     The model matrix (from model_matrix())
    // Attributes:
    // 0 - position, 1 - color, 2 - tex coord
    void set_shader(sf::Shader* shader) { m_shader = shader; }

    void set_model_matrix(Matrix4x4d matrix) { m_model_matrix = matrix; }
    Matrix4x4d model_matrix() const { return m_model_matrix; }

    class ModelScope {
    public:
        ModelScope(SFMLWindow& wnd, Matrix4x4d mat)
            : m_window(wnd)
            , m_old_matrix(wnd.model_matrix()) {
            wnd.set_model_matrix(mat);
        }
        ~ModelScope() {
            m_window.set_model_matrix(m_old_matrix);
        }

    private:
        SFMLWindow& m_window;
        Matrix4x4d m_old_matrix;
    };

    void clear(sf::Color = sf::Color::Black);

    void draw_vertices(GLenum mode, std::span<Vertex const>);
    void draw_outline(std::span<Vector3 const>, Color color, float thickness);
    void draw_indexed_vertices(GLenum mode, std::span<Vertex const>, std::span<unsigned const> indices);

    void draw_rectangle(sf::FloatRect bounds, RectangleDrawOptions const& = {});
    void draw_text(sf::String const&, sf::Font const&, sf::Vector2f position, TextDrawOptions const& = {});
    void draw_text_aligned_in_rect(sf::String const&, sf::FloatRect rect, sf::Font const&, TextDrawOptions const& = {});
    void draw_ellipse(sf::Vector2f center, sf::Vector2f size, DrawOptions const& = {});

    // FIXME: Add some class like sf::Text.
    sf::Vector2f calculate_text_size(sf::String const&, sf::Font const&, TextDrawOptions const& = {});
    sf::Vector2f find_character_position(size_t index, sf::String const&, sf::Font const&, TextDrawOptions const& = {});

private:
    void apply_states();
    void ensure_vao_vbo();

    sf::View m_view;
    Matrix4x4d m_model_matrix = Matrix4x4d::identity();
    sf::Texture const* m_texture = nullptr;
    sf::Shader* m_shader = nullptr;
    unsigned m_temporary_vao = 0;
    unsigned m_temporary_vbo = 0;
};

}
