#include <Essa/Engine/3D/Model.hpp>
#include <Essa/Engine/3D/ObjLoader.hpp>
#include <Essa/Engine/3D/Shaders/Basic.hpp>
#include <Essa/Engine/3D/Shaders/Lighting.hpp>
#include <Essa/GUI/Application.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/OpenGL.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <Essa/LLGL/Window/Keyboard.hpp>
#include <Essa/LLGL/Window/Window.hpp>
#include <EssaUtil/Angle.hpp>
#include <EssaUtil/Color.hpp>
#include <iostream>

int main() {
    // FIXME: (Hack) GUI::Application is required for ResourceManager to work, which is
    //        required by Lighting shader now (Try to find a way to fix this!)
    GUI::Application app;

    llgl::Window window { { 500, 500 }, "OBJ loader", { 3, 2 } };

    // TODO: Port to llgl, especially clear color.
    glEnable(GL_DEPTH_TEST);
    glClearColor(1, 0.5, 0.5, 1);

    auto object = Essa::ObjLoader::load_object_from_file("../examples/essa-engine/ladyball.obj");
    if (!object.has_value()) {
        std::cerr << "FAILED TO READ :((" << std::endl;
        return 1;
    }

    llgl::Transform model_transform;
    model_transform = model_transform.translate({ -1.5, -1.5, -5 }).rotate_y(Util::deg_to_rad(45));

    Essa::Shaders::Lighting lighting_shader;

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

        // TODO: Port to llgl.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        llgl::set_viewport(window.rect());

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
        lighting_shader.set_light_position({ static_cast<float>(std::sin(light_angle)), 5, static_cast<float>(std::cos(light_angle)) });

        {
            lighting_shader.set_transform(model_transform.matrix(),
                view_transform.matrix(),
                llgl::Projection::perspective({ 1.22, window.aspect(), 0.1, 20 }, {}).matrix());
            lighting_shader.set_light_color(Util::Colors::Red * 0.8);
            object->render(window.renderer(), lighting_shader);
        }

        {
            lighting_shader.set_transform(model_transform.translate({ 3, 0, 0 }).matrix(),
                view_transform.matrix(),
                llgl::Projection::perspective({ 1.22, window.aspect(), 0.1, 20 }, {}).matrix());
            lighting_shader.set_light_color(Util::Colors::Blue * 0.8);
            object->render(window.renderer(), lighting_shader);
        }

        window.display();
    }
    return 0;
}
