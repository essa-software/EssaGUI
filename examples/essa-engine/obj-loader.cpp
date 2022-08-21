#include <EssaEngine/3D/ObjLoader.hpp>
#include <EssaEngine/3D/Object3D.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Color.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Shaders/Basic330Core.hpp>
#include <LLGL/OpenGL/Shaders/ShadeFlat.hpp>
#include <LLGL/OpenGL/Utils.hpp>
#include <LLGL/Renderer/BatchRenderer.hpp>
#include <LLGL/Renderer/Renderer.hpp>
#include <LLGL/Renderer/Transform.hpp>
#include <LLGL/Window/Keyboard.hpp>
#include <LLGL/Window/Window.hpp>
#include <iostream>

int main() {
    llgl::Window window { { 500, 500 }, "OBJ loader", { 3, 2 } };

    llgl::opengl::enable(llgl::opengl::Feature::DepthTest);
    llgl::opengl::set_clear_color(Util::Color { 255, 128, 128 });

    auto object = Essa::ObjLoader::load_object_from_file("../ladyball.obj");
    if (!object.has_value()) {
        std::cerr << "FAILED TO READ :((" << std::endl;
        return 1;
    }

    llgl::Transform model_transform;
    model_transform = model_transform.translate({ -1.5, -1.5, -5 }).rotate_y(Util::deg_to_rad(45));

    llgl::opengl::shaders::ShadeFlat shader;

    double light_angle = 0;

    bool a_pressed = false;
    bool d_pressed = false;
    bool w_pressed = false;
    bool s_pressed = false;

    Util::Vector3f camera_position;
    double yaw = 0;
    double pitch = 0;

    for (;;) {
        llgl::Event event;
        while (window.poll_event(event)) {
            switch (event.type) {
            case llgl::Event::Type::KeyPress:
                std::cout << "key press: " << llgl::to_string(event.key.keycode) << std::endl;
                if (event.key.keycode == llgl::KeyCode::A)
                    a_pressed = true;
                else if (event.key.keycode == llgl::KeyCode::D)
                    d_pressed = true;
                else if (event.key.keycode == llgl::KeyCode::W)
                    w_pressed = true;
                else if (event.key.keycode == llgl::KeyCode::S)
                    s_pressed = true;
                break;
            case llgl::Event::Type::KeyRelease:
                std::cout << "key release: " << llgl::to_string(event.key.keycode) << std::endl;
                if (event.key.keycode == llgl::KeyCode::A)
                    a_pressed = false;
                else if (event.key.keycode == llgl::KeyCode::D)
                    d_pressed = false;
                else if (event.key.keycode == llgl::KeyCode::W)
                    w_pressed = false;
                else if (event.key.keycode == llgl::KeyCode::S)
                    s_pressed = false;
                break;
            case llgl::Event::Type::MouseMove:
                yaw += Util::deg_to_rad<float>(event.mouse_move.relative.x);
                pitch += Util::deg_to_rad<float>(event.mouse_move.relative.y);
                break;
            default:
                break;
            }
        }
        llgl::opengl::clear(llgl::opengl::ClearMask::Color | llgl::opengl::ClearMask::Depth);

        window.renderer().apply_projection(llgl::Projection::perspective({ 1.22, window.aspect(), 0.1, 20 }, window.rect()));
        model_transform = model_transform.rotate_x(0.05);

        if (a_pressed)
            camera_position.x() += 0.1;
        if (d_pressed)
            camera_position.x() -= 0.1;
        if (w_pressed)
            camera_position.z() += 0.1;
        if (s_pressed)
            camera_position.z() -= 0.1;
        llgl::Transform view_transform;
        view_transform = view_transform.rotate_y(yaw).rotate_x(pitch).translate(camera_position);

        light_angle += 0.01;
        shader.set_light_position({ static_cast<float>(std::sin(light_angle)), 5, static_cast<float>(std::cos(light_angle)) });

        {
            shader.set_light_color(Util::Colors::Red * 0.8);
            window.renderer().render_object(object.value(), {
                                                                .shader = &shader,
                                                                .model_matrix = model_transform.matrix(),
                                                                .view_matrix = view_transform.matrix(),
                                                            });
        }

        {
            shader.set_light_color(Util::Colors::Green * 0.8);
            window.renderer().render_object(object.value(), {
                                                                .shader = &shader,
                                                                .model_matrix = model_transform.translate({ 3, 0, 0 }).matrix(),
                                                                .view_matrix = view_transform.matrix(),
                                                            });
        }

        window.display();
    }
    return 0;
}
