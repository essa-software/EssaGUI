#include <Essa/Engine/3D/Model.hpp>
#include <Essa/Engine/3D/Shaders/Lighting.hpp>
#include <Essa/GUI/Application.hpp>

#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <Essa/GUI/Widgets/WorldView.hpp>
#include <Essa/LLGL/Core/Transform.hpp>

class WorldView : public GUI::WorldView {
private:
    llgl::Camera camera() const {
        return llgl::Camera {
            llgl::Projection::perspective(
                { 1.44, static_cast<double>(raw_size().x() / raw_size().y()),
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
        Essa::Shaders::Lighting::Uniforms uniforms;
        uniforms.set_light_color(Util::Colors::LightGoldenRodYellow);
        uniforms.set_light_position({ 0, 5, 10 });

        auto& model = resource_manager().require_external<Essa::Model>(
            "../examples/essa-engine/multiple-materials.obj");

        auto transform = llgl::Transform {}
                             .rotate_x(m_angle_x.rad())
                             .rotate_y(m_angle_y.rad())
                             .rotate_z(m_angle_z.rad());
        uniforms.set_transform(transform.matrix(), camera().view_matrix(),
            camera().projection().matrix());
        model.render(painter.renderer(), shader, uniforms);
    }

    Util::Angle m_angle_x;
    Util::Angle m_angle_y;
    Util::Angle m_angle_z;
};

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window(
        { 500, 500 }, "Multiple Materials support in Model");
    host_window.set_root_widget<WorldView>();
    host_window.set_background_color(Util::Colors::Black);
    app.run();
    return 0;
}
