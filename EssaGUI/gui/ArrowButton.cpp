#include "ArrowButton.hpp"

#include <EssaGUI/gfx/Window.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <iostream>

namespace GUI {

void ArrowButton::draw(GUI::Window& window) const {
    RectangleDrawOptions options;
    options.fill_color = bg_color_for_state();
    window.draw_rectangle({ {}, size() }, options);

    const Util::Vector2f midpoint = size() / 2.f;

    // std::cout << Vector3(midpoint) << "\n";

    const Util::Vector2f points[8] = {
        Util::Vector2f(std::round(midpoint.x() - m_arrow_size / 2), std::round(midpoint.y() - m_arrow_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + 0), std::round(midpoint.y() - m_arrow_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + m_arrow_size / 2), std::round(midpoint.y() - m_arrow_size / 2)),
        Util::Vector2f(std::round(midpoint.x() - m_arrow_size / 2), std::round(midpoint.y() + 0)),
        Util::Vector2f(std::round(midpoint.x() + m_arrow_size / 2), std::round(midpoint.y() + 0)),
        Util::Vector2f(std::round(midpoint.x() - m_arrow_size / 2), std::round(midpoint.y() + m_arrow_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + 0), std::round(midpoint.y() + m_arrow_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + m_arrow_size / 2), std::round(midpoint.y() + m_arrow_size / 2))
    };

    std::array<llgl::Vertex, 3> arrow;
    switch (m_arrow_type) {
    case ArrowType::LEFTARROW:
        arrow[0] = llgl::Vertex { .position = Util::Vector3f(points[2], 0), .color = m_arrow_color };
        arrow[1] = llgl::Vertex { .position = Util::Vector3f(points[3], 0), .color = m_arrow_color };
        arrow[2] = llgl::Vertex { .position = Util::Vector3f(points[7], 0), .color = m_arrow_color };
        break;
    case ArrowType::BOTTOMARROW:
        arrow[0] = llgl::Vertex { .position = Util::Vector3f(points[0], 0), .color = m_arrow_color };
        arrow[1] = llgl::Vertex { .position = Util::Vector3f(points[6], 0), .color = m_arrow_color };
        arrow[2] = llgl::Vertex { .position = Util::Vector3f(points[2], 0), .color = m_arrow_color };
        break;
    case ArrowType::RIGHTARROW:
        arrow[0] = llgl::Vertex { .position = Util::Vector3f(points[0], 0), .color = m_arrow_color };
        arrow[1] = llgl::Vertex { .position = Util::Vector3f(points[4], 0), .color = m_arrow_color };
        arrow[2] = llgl::Vertex { .position = Util::Vector3f(points[5], 0), .color = m_arrow_color };
        break;
    case ArrowType::TOPARROW:
        arrow[0] = llgl::Vertex { .position = Util::Vector3f(points[5], 0), .color = m_arrow_color };
        arrow[1] = llgl::Vertex { .position = Util::Vector3f(points[1], 0), .color = m_arrow_color };
        arrow[2] = llgl::Vertex { .position = Util::Vector3f(points[7], 0), .color = m_arrow_color };
        break;
    }
    window.draw_vertices(llgl::opengl::PrimitiveType::Triangles, arrow);
}

}
