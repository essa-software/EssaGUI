#include <EssaEngine/3D/Model.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/WorldView.hpp>
#include <LLGL/OpenGL/Shaders/Basic330Core.hpp>
#include <LLGL/OpenGL/Shaders/ShadeFlat.hpp>

class WorldView : public GUI::WorldView {
public:
    explicit WorldView(GUI::Container& parent)
        : GUI::WorldView(parent) {
        set_background_color(Util::Colors::LightBlue);
    }

private:
    virtual llgl::Camera camera() const override {
        return llgl::Camera { llgl::Projection::perspective({ 1.44, window().aspect(), 0.1, 20 }, window().rect()) }
            .translate({ 0, 3, 3 })
            .rotate_x(45.0_deg)
            .translate({ 0, 0, -2 });
    }

    virtual void update() override {
        m_angle += 0.1_rad;
    }

    virtual void draw(GUI::Window& window) const override {
        GUI::WorldDrawScope scope { *this, GUI::WorldDrawScope::ClearDepth::Yes };

        static llgl::opengl::shaders::ShadeFlat shader;
        shader.set_light_color(Util::Colors::LightGoldenRodYellow);

        auto& model = resource_manager().require_external<Essa::Model>("../examples/llgl/ladyball.obj");

        auto transform = llgl::Transform {}.rotate_x(m_angle.rad());

        window.renderer().render_object(model, { .shader = &shader, .model_matrix = transform.matrix(), .view_matrix = window.view_matrix() });
    }

    Util::Angle m_angle;
};

int main() {

    GUI::Window window { { 500, 500 }, "Model ResourceManager integration" };

    GUI::Application app { window };

    auto& container = app.set_main_widget<GUI::Container>();
    container.set_layout<GUI::HorizontalBoxLayout>();

    container.add_widget<WorldView>();

    app.run();
    return 0;
}
