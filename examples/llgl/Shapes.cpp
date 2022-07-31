#include <EssaUtil/Angle.hpp>
#include <LLGL/3D/Cube.hpp>
#include <LLGL/3D/Sphere.hpp>
#include <LLGL/OpenGL/Shaders/ShadeFlat.hpp>
#include <LLGL/OpenGL/Utils.hpp>
#include <LLGL/Renderer/Transform.hpp>
#include <LLGL/Window/Window.hpp>

int main()
{
    llgl::Window window { { 500, 500 }, "Primitive Shapes", { 3, 2 } };

    llgl::opengl::enable(llgl::opengl::Feature::DepthTest);
    llgl::opengl::set_clear_color(Util::Color { 255, 128, 128 });

    llgl::opengl::shaders::ShadeFlat shader;
    llgl::Sphere sphere;
    llgl::Cube cube;

    double light_angle = 0;
    double shape_angle = 0;

    for (;;) {
        llgl::Event event;
        while (window.poll_event(event)) {
        }

        window.renderer().clear();

        llgl::View view;
        view.set_viewport(window.rect());
        view.set_perspective({ 1.22, window.aspect(), 0.1, 20 });
        window.renderer().apply_view(view);

        llgl::Transform view_transform = llgl::Transform {}.translate({ 0, -1.5, -5 });

        light_angle += 0.01;
        shape_angle += 0.05;

        shader.set_light_position({ static_cast<float>(std::sin(light_angle)), 5, static_cast<float>(std::cos(light_angle)) });

        {
            shader.set_light_color(Util::Colors::Red * 0.8);
            window.renderer().render_object(sphere, {
                                                        .shader = &shader,
                                                        .model_matrix = llgl::Transform {}.translate({ -1.5, 0, 0 }).rotate_x(shape_angle).matrix(),
                                                        .view_matrix = view_transform.matrix(),
                                                    });
        }

        {
            shader.set_light_color(Util::Colors::Green * 0.8);
            window.renderer().render_object(sphere, {
                                                        .shader = &shader,
                                                        .model_matrix = llgl::Transform {}.translate({ 1.5, 0, 0 }).rotate_x(shape_angle).matrix(),
                                                        .view_matrix = view_transform.matrix(),
                                                    });
        }

        {
            shader.set_light_color(Util::Colors::Blue * 0.8);
            window.renderer().render_object(cube, {
                                                      .shader = &shader,
                                                      .model_matrix = llgl::Transform {}.scale(0.7).translate({ 0, 3.5, 0 }).rotate_x(shape_angle).rotate_y(Util::deg_to_rad(45.0)).matrix(),
                                                      .view_matrix = view_transform.matrix(),
                                                  });
        }

        window.display();
    }
    return 0;
}
