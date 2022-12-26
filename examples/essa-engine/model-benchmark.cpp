#include <Essa/Engine/3D/Model.hpp>
#include <Essa/Engine/3D/Shaders/Lighting.hpp>
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <Essa/GUI/Widgets/WorldView.hpp>
#include <EssaUtil/Random.hpp>

// ~2.4 FPS
// ~2.7-~3.0 FPS after optimizing transforms
// ~6.1-~6.8 FPS after uniform location caching

constexpr size_t ModelCount = 100000;

class WorldView : public GUI::Widget {
public:
    WorldView()
        : m_model(GUI::Application::the().resource_manager().require_external<Essa::Model>("../examples/essa-engine/tree.obj")) {
    }

private:
    virtual void draw(Gfx::Painter& painter) const override {
        GUI::WorldDrawScope scope { painter };

        llgl::Camera camera { llgl::Projection::perspective({ 1.4, raw_size().x() / raw_size().y(), 0.1, 30 }, {}) };
        camera = camera.translate({ 0, 2, 6 });

        Essa::Shaders::Lighting::Uniforms uniforms;
        uniforms.set_transform({}, camera.view_matrix(), camera.projection().matrix());

        for (size_t s = 0; s < ModelCount; s++) {
            auto uniforms_model = uniforms;
            auto rndx = Util::Random::floating<float>(-4, 4);
            auto rndy = Util::Random::floating<float>(-22, 2);
            uniforms_model.set_model(llgl::Transform {}.translate({ rndx, 0, rndy }).scale(0.05).matrix());
            m_model.render(painter.renderer(), m_shader, uniforms_model);
        }
    };

    Essa::Model const& m_model;
    mutable Essa::Shaders::Lighting m_shader;
};

int main() {
    GUI::SimpleApplication<GUI::Container> app { "Essa::Model rendering benchmark" };

    auto& main_widget = app.main_widget();
    main_widget.set_layout<GUI::BasicLayout>();

    auto wv = main_widget.add_widget<WorldView>();
    wv->set_size({ 100.0_perc, 100.0_perc });

    auto label = main_widget.add_widget<GUI::Textfield>();
    label->set_size({ 400.0_px, 50.0_px });

    app.on_tick = [&]() {
        label->set_content(Util::to_ustring(app.tps()));
    };

    app.run();
    return 0;
}
