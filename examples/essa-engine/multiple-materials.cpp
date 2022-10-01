#include <EssaEngine/3D/Model.hpp>
#include <EssaEngine/3D/Shaders/Lighting.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaGUI/gui/WidgetTreeRoot.hpp>
#include <EssaGUI/gui/WorldView.hpp>
#include <LLGL/Core/Transform.hpp>

class WorldView : public GUI::WorldView {
private:
    llgl::Camera camera() const {
        return llgl::Camera { llgl::Projection::perspective({ 1.44, raw_size().x() / raw_size().y(), 0.1, 20 }, Util::Recti { rect() }) }
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
        GUI::WorldDrawScope scope { painter, GUI::WorldDrawScope::ClearDepth::Yes };

        static Essa::Shaders::Lighting shader;
        shader.set_light_color(Util::Colors::LightGoldenRodYellow);
        shader.set_light_position({ 0, 5, 10 });

        auto& model = resource_manager().require_external<Essa::Model>("../examples/essa-engine/multiple-materials.obj");

        auto transform = llgl::Transform {}
                             .rotate_x(m_angle_x.rad())
                             .rotate_y(m_angle_y.rad())
                             .rotate_z(m_angle_z.rad());
        shader.set_transform(transform.matrix(), camera().view_matrix(), camera().projection().matrix());
        model.render(painter.renderer(), shader);
    }

    Util::Angle m_angle_x;
    Util::Angle m_angle_y;
    Util::Angle m_angle_z;
};

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Multiple Materials support in Model");
    host_window.set_main_widget<WorldView>();
    app.run();
    return 0;
}
