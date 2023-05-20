#include <Essa/Engine/3D/Model.hpp>
#include <Essa/Engine/3D/Shaders/Lighting.hpp>
#include <Essa/GUI/Application.hpp>

#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <Essa/GUI/Widgets/WorldView.hpp>
#include <Essa/LLGL/Core/Transform.hpp>

class WorldView : public GUI::WorldView {
public:
    explicit WorldView()
        : GUI::WorldView() {
        // FIXME: The "background" would now cover cubes because it is rendered
        // by Builder AFTER draw() is called. Make background rendered
        // immediately for WorldView so that is is actually *below* the
        // world render.
        // set_background_color(Util::Colors::LightBlue);
    }

private:
    llgl::Camera camera() const {
        return llgl::Camera {
            llgl::Projection::perspective(
                { 1.44, static_cast<double>(raw_size().x()) / raw_size().y(),
                    0.1, 20 },
                Util::Recti { local_rect() })
        }
            .translate({ 0, 3, 3 })
            .rotate_x(45.0_deg)
            .translate({ 0, 0, -1 });
    }

    virtual void update() override {
        m_angle_x += 1.0_deg;
        m_angle_y += 1.0_deg;
        m_angle_z += 1.0_deg;
    }

    virtual void draw(Gfx::Painter& painter) const override {
        GUI::WorldDrawScope scope { painter,
            GUI::WorldDrawScope::ClearDepth::Yes };

        static Essa::Shaders::Lighting shader;
        Essa::Shaders::Lighting::Uniforms shader_uniforms;
        shader_uniforms.set_light_color(Util::Colors::LightGoldenRodYellow);

        auto& model = resource_manager().require_external<Essa::Model>(
            "../examples/essa-engine/cube.obj");

        auto transform = llgl::Transform {}
                             .rotate_x(m_angle_x.rad())
                             .rotate_y(m_angle_y.rad())
                             .rotate_z(m_angle_z.rad());
        shader_uniforms.set_transform(transform.matrix(),
            camera().view_matrix(), camera().projection().matrix());
        model.render(painter.renderer(), shader, shader_uniforms);
    }

    Util::Angle m_angle_x;
    Util::Angle m_angle_y;
    Util::Angle m_angle_z;
};

class MainWidget : public GUI::Container {
public:
    virtual void on_init() override {
        set_layout<GUI::VerticalBoxLayout>();

        m_tps_container = add_widget<GUI::Textfield>();

        auto views_container = add_widget<GUI::Container>();
        views_container->set_layout<GUI::HorizontalBoxLayout>();

        views_container->add_widget<WorldView>();
        views_container->add_widget<WorldView>();
    }

private:
    virtual void update() override {
        m_tps_container->set_content(Util::UString {
            "TPS: " + std::to_string(GUI::Application::the().tps()) });
    }

    GUI::Textfield* m_tps_container = nullptr;
};

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Text Editor");
    host_window.set_main_widget<MainWidget>();
    app.run();
    return 0;
}
