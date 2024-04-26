#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/DefaultGUIShader.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/ShaderResource.hpp>
#include <EssaUtil/Clock.hpp>

class Shader {
public:
    using Vertex = Gfx::Vertex;

    struct Uniforms : public Gfx::DefaultGUIShader::Uniforms {
        float time = 0;
        float size = 0; // just to make Uniforms bigger to test our Any impl

        static inline auto mapping = Gfx::DefaultGUIShader::Uniforms::mapping | llgl::Uniform { "time", &Uniforms::time }
            | llgl::Uniform { "size", &Uniforms::size };
    };
    static_assert(sizeof(Uniforms) > sizeof(Gfx::DefaultGUIShader::Uniforms));
};

class MainWidget : public GUI::Widget {
private:
    virtual void on_init() override {
        m_shader = &resource_manager().require<Gfx::FullShaderResource<Shader>>("Shader.shader");
    }
    virtual void draw(Gfx::Painter& painter) const override {
        using namespace Gfx::Drawing;
        using namespace std::chrono_literals;
        auto time = static_cast<float>(m_clock.elapsed() / 100.0ms);
        painter.draw(
            Rectangle(local_rect().inflated(-10).cast<float>(), Fill::solid(Util::Colors::Red)).set_shader(*m_shader, { {}, time, time })
        );
    }

    Gfx::FullShaderResource<Shader>* m_shader;
    Util::Clock m_clock;
};

int main() {
    GUI::SimpleApplication<MainWidget> app("Shaders");
    app.window().set_background_color(Util::Colors::Black);
    app.run();
    return 0;
}
