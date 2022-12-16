#include <Essa/Engine/3D/Cube.hpp>
#include <Essa/Engine/3D/Shaders/Lighting.hpp>
#include <Essa/Engine/3D/Sphere.hpp>
#include <Essa/LLGL/Core/Camera.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/Angle.hpp>

int main() {
    llgl::Window window { { 500, 500 }, "Primitive Shapes" };

    glEnable(GL_DEPTH_TEST);

    Essa::Shaders::Lighting shader;
    Essa::Sphere sphere;
    Essa::Cube cube;

    double light_angle = 0;
    double shape_angle = 0;

    for (;;) {
        // FIXME: Port to GUI::Application
        while (window.poll_event()) { }

        llgl::Camera camera { llgl::Projection::perspective({ 1.22, window.aspect(), 0.1, 20 }, window.rect()) };
        camera = camera.translate({ 0, 1.5, 5 }).rotate_z(10.0_deg);

        window.renderer().clear(Util::Color { 255, 128, 128 });
        llgl::set_viewport(camera.projection().viewport());
        glClear(GL_DEPTH_BUFFER_BIT);

        light_angle += 0.01;
        shape_angle += 0.05;

        Essa::Shaders::Lighting::Uniforms uniforms_base;
        uniforms_base.set_light_position({ static_cast<float>(std::sin(light_angle)), 5, static_cast<float>(std::cos(light_angle)) });

        {
            Essa::Shaders::Lighting::Uniforms uniforms = uniforms_base;
            uniforms.set_light_color(Util::Colors::Red * 0.8);
            uniforms.set_transform(llgl::Transform {}.translate({ -1.5, 0, 0 }).rotate_x(shape_angle).matrix(),
                camera.view_matrix(), camera.projection().matrix());
            sphere.render(window.renderer(), shader, uniforms);
        }

        {
            Essa::Shaders::Lighting::Uniforms uniforms = uniforms_base;
            uniforms.set_light_color(Util::Colors::Green * 0.8);
            uniforms.set_transform(llgl::Transform {}.translate({ 1.5, 0, 0 }).rotate_x(shape_angle).matrix(),
                camera.view_matrix(), camera.projection().matrix());
            sphere.render(window.renderer(), shader, uniforms);
        }

        {
            Essa::Shaders::Lighting::Uniforms uniforms = uniforms_base;
            uniforms.set_light_color(Util::Colors::Blue * 0.8);
            uniforms.set_transform(llgl::Transform {}.scale(0.7).translate({ 0, 3.5, 0 }).rotate_x(shape_angle).rotate_y(Util::deg_to_rad(45.0)).matrix(),
                camera.view_matrix(), camera.projection().matrix());
            cube.render(window.renderer(), shader, uniforms);
        }

        window.display();
    }
    return 0;
}
