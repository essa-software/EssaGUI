#include "ArrowButton.hpp"

#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <iostream>

namespace GUI {

void ArrowButton::draw(Gfx::Painter& painter) const {
    auto colors = colors_for_state();

    Gfx::RectangleDrawOptions options;
    options.fill_color = colors.background;
    painter.deprecated_draw_rectangle({ {}, raw_size() }, options);

    const Util::Vector2f midpoint = raw_size() / 2.f;

    // std::cout << Vector3(midpoint) << "\n";

    double real_size = m_arrow_size.value();

    const Util::Vector2f points[8] = {
        Util::Vector2f(std::round(midpoint.x() - real_size / 2), std::round(midpoint.y() - real_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + 0), std::round(midpoint.y() - real_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + real_size / 2), std::round(midpoint.y() - real_size / 2)),
        Util::Vector2f(std::round(midpoint.x() - real_size / 2), std::round(midpoint.y() + 0)),
        Util::Vector2f(std::round(midpoint.x() + real_size / 2), std::round(midpoint.y() + 0)),
        Util::Vector2f(std::round(midpoint.x() - real_size / 2), std::round(midpoint.y() + real_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + 0), std::round(midpoint.y() + real_size / 2)),
        Util::Vector2f(std::round(midpoint.x() + real_size / 2), std::round(midpoint.y() + real_size / 2))
    };

    std::array<Gfx::Vertex, 3> arrow;

    auto arrow_color = colors.text;

    switch (m_arrow_type) {
    case ArrowType::LEFTARROW:
        arrow[0] = Gfx::Vertex { points[2], arrow_color, {} };
        arrow[1] = Gfx::Vertex { points[3], arrow_color, {} };
        arrow[2] = Gfx::Vertex { points[7], arrow_color, {} };
        break;
    case ArrowType::BOTTOMARROW:
        arrow[0] = Gfx::Vertex { points[0], arrow_color, {} };
        arrow[1] = Gfx::Vertex { points[6], arrow_color, {} };
        arrow[2] = Gfx::Vertex { points[2], arrow_color, {} };
        break;
    case ArrowType::RIGHTARROW:
        arrow[0] = Gfx::Vertex { points[0], arrow_color, {} };
        arrow[1] = Gfx::Vertex { points[4], arrow_color, {} };
        arrow[2] = Gfx::Vertex { points[5], arrow_color, {} };
        break;
    case ArrowType::TOPARROW:
        arrow[0] = Gfx::Vertex { points[5], arrow_color, {} };
        arrow[1] = Gfx::Vertex { points[1], arrow_color, {} };
        arrow[2] = Gfx::Vertex { points[7], arrow_color, {} };
        break;
    }
    // TODO: Take advantage of GUIBuilder
    painter.draw_vertices(llgl::PrimitiveType::Triangles, arrow);
}

EML::EMLErrorOr<void> ArrowButton::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Button::load_from_eml_object(object, loader));
    m_arrow_size = TRY(object.get_property("arrow_size", 8.0_px).to_length());
    auto type = TRY(object.get_property("arrow_type", Util::UString { "left" }).to_string());

    if (type == "left") {
        m_arrow_type = ArrowType::LEFTARROW;
    }
    else if (type == "bottom") {
        m_arrow_type = ArrowType::BOTTOMARROW;
    }
    else if (type == "right") {
        m_arrow_type = ArrowType::RIGHTARROW;
    }
    else if (type == "top") {
        m_arrow_type = ArrowType::TOPARROW;
    }

    return {};
}

EML_REGISTER_CLASS(ArrowButton);

}
