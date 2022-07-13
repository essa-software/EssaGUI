#pragma once

#include <EssaGUI/glwrapper/Vertex.hpp>
#include <EssaUtil/Rect.hpp>
#include <EssaUtil/UString.hpp>
#include <GL/glew.h>

#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Matrix.hpp>
#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <span>

namespace GUI {

struct DrawOptions {
    Util::Color fill_color = Util::Colors::white;
    Util::Color outline_color = Util::Colors::white;
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
    SFMLWindow(sf::VideoMode, Util::UString title, unsigned style = sf::Style::Default, sf::ContextSettings = sf::ContextSettings {});
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

    void set_model_matrix(Util::Matrix4x4d matrix) { m_model_matrix = matrix; }
    Util::Matrix4x4d model_matrix() const { return m_model_matrix; }

    class ModelScope {
    public:
        ModelScope(SFMLWindow& wnd, Util::Matrix4x4d mat)
            : m_window(wnd)
            , m_old_matrix(wnd.model_matrix()) {
            wnd.set_model_matrix(mat);
        }
        ~ModelScope() {
            m_window.set_model_matrix(m_old_matrix);
        }

    private:
        SFMLWindow& m_window;
        Util::Matrix4x4d m_old_matrix;
    };

    void clear(Util::Color = Util::Colors::black);

    void draw_vertices(GLenum mode, std::span<Vertex const>);
    void draw_outline(std::span<Util::Vector3f const>, Util::Color color, float thickness);
    void draw_indexed_vertices(GLenum mode, std::span<Vertex const>, std::span<unsigned const> indices);

    void draw_rectangle(Util::Rectf bounds, RectangleDrawOptions const& = {});
    void draw_text(Util::UString const&, sf::Font const&, Util::Vector2f position, TextDrawOptions const& = {});
    void draw_text_aligned_in_rect(Util::UString const&, Util::Rectf rect, sf::Font const&, TextDrawOptions const& = {});
    void draw_ellipse(Util::Vector2f center, Util::Vector2f size, DrawOptions const& = {});

    // FIXME: Add some class like sf::Text.
    Util::Vector2f calculate_text_size(Util::UString const&, sf::Font const&, TextDrawOptions const& = {});
    Util::Vector2f find_character_position(size_t index, Util::UString const&, sf::Font const&, TextDrawOptions const& = {});

private:
    void apply_states();
    void ensure_vao_vbo();

    sf::View m_view;
    Util::Matrix4x4d m_model_matrix = Util::Matrix4x4d::identity();
    sf::Texture const* m_texture = nullptr;
    sf::Shader* m_shader = nullptr;
    unsigned m_temporary_vao = 0;
    unsigned m_temporary_vbo = 0;
};

}
