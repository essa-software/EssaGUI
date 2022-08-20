#include "LLGL/Core/Vector3.hpp"
#include <EssaUtil/Color.hpp>
#include <LLGL/OpenGL/Utils.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/Renderer/BatchRenderer.hpp>
#include <LLGL/Renderer/Renderer.hpp>
#include <LLGL/Window/Window.hpp>
#include <iostream>

int main()
{
    int const edges = 13;
    float const radius = 400;
    float const width = 50;
    llgl::Window window { { 500, 500 }, "impossible shapes", { 3, 2 } };
    llgl::BatchRenderer batch_renderer { window };

    {
        for (size_t s = 0; s < edges; s++)
        {
            llgl::DrawScope scope { batch_renderer, { llgl::opengl::PrimitiveType::TriangleStrip } };
            float const step = 2 * M_PI / edges;
            float const start_angle = step * s;

            auto calculate_point_1 = [&](int offset)
            {
                return llgl::Vector3f {
                    radius * std::cos(start_angle - offset * step) + width / 2 * std::cos(start_angle - offset * step + (float)M_PI_2),
                    radius * std::sin(start_angle - offset * step) + width / 2 * std::sin(start_angle - offset * step + (float)M_PI_2), 0
                };
            };

            auto calculate_point_2 = [&](int offset)
            {
                return llgl::Vector3f {
                    radius * std::cos(start_angle - offset * step) - width / 2 * std::cos(start_angle - offset * step + (float)M_PI_2),
                    radius * std::sin(start_angle - offset * step) - width / 2 * std::sin(start_angle - offset * step + (float)M_PI_2), 0
                };
            };

            auto start_point1 = calculate_point_1(0);
            auto start_point2 = calculate_point_2(0);
            auto mid_point2 = calculate_point_1(1);
            auto end_point1 = calculate_point_1(2) + (mid_point2 - calculate_point_2(1));
            auto end_point2 = end_point1 + (calculate_point_1(-edges / 6) - calculate_point_2(-edges / 6));

            batch_renderer.add_vertexes(std::initializer_list<llgl::Vertex> {
                { start_point1, Util::Colors::Green },
                { start_point2, Util::Colors::Green },
                { mid_point2 - (start_point2 - start_point1), Util::Colors::Blue },
                { mid_point2, Util::Colors::Green },
                { end_point2, Util::Colors::Red },
                { end_point1, Util::Colors::Green },
            });
        }
    }

    for (;;)
    {
        llgl::Event event;
        while (window.poll_event(event))
        {
        }
        llgl::opengl::clear(llgl::opengl::ClearMask::Color);

        llgl::View view;
        view.set_viewport(llgl::Recti { 0, 0, window.size().x, window.size().y });
        view.set_ortho(llgl::Rectd { static_cast<double>(-window.size().x / 2.0), static_cast<double>(-window.size().y / 2.0),
            static_cast<double>(window.size().x), static_cast<double>(window.size().y) });
        window.renderer().apply_view(view);
        window.renderer().render_object(batch_renderer);
        window.display();
    }
    return 0;
}
