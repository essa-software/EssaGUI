#include "EssaGUI/gui/WidgetTreeRoot.hpp"
#include <EssaEngine/3D/Model.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
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
        return llgl::Camera { llgl::Projection::perspective({ 1.44, size().x() / size().y(), 0.1, 20 }, window().rect()) }
            .translate({ 0, 3, 3 })
            .rotate_x(45.0_deg)
            .translate({ 0, 0, -1 });
    }

    virtual void update() override {
        m_angle_x += 1.0_deg;
        m_angle_y += 1.0_deg;
        m_angle_z += 1.0_deg;
    }

    virtual void draw(GUI::Window& window) const override {
        GUI::WorldDrawScope scope { *this, GUI::WorldDrawScope::ClearDepth::Yes };

        static llgl::opengl::shaders::ShadeFlat shader;
        shader.set_light_color(Util::Colors::LightGoldenRodYellow);

        auto& model = resource_manager().require_external<Essa::Model>("../examples/essa-engine/cube.obj");

        auto transform = llgl::Transform {}
                             .rotate_x(m_angle_x.rad())
                             .rotate_y(m_angle_y.rad())
                             .rotate_z(m_angle_z.rad());

        window.renderer().render_object(model, { .shader = &shader, .model_matrix = transform.matrix(), .view_matrix = window.view_matrix() });
    }

    Util::Angle m_angle_x;
    Util::Angle m_angle_y;
    Util::Angle m_angle_z;
};

class MainWidget : public GUI::Container {
public:
    MainWidget(GUI::WidgetTreeRoot& wtr)
        : GUI::Container(wtr) {
        set_layout<GUI::VerticalBoxLayout>();

        m_tps_container = add_widget<GUI::Textfield>();

        auto views_container = add_widget<GUI::Container>();
        views_container->set_layout<GUI::HorizontalBoxLayout>();

        views_container->add_widget<WorldView>();
        views_container->add_widget<WorldView>();
    }

private:
    virtual void update() override {
        m_tps_container->set_content(Util::UString { "TPS: " + std::to_string(GUI::Application::the().tps()) });
    }

    GUI::Textfield* m_tps_container = nullptr;
};

int main() {

    GUI::Window window { { 500, 500 }, "Model ResourceManager integration" };

    GUI::Application app { window };
    app.set_main_widget<MainWidget>();

    app.run();
    return 0;
}
