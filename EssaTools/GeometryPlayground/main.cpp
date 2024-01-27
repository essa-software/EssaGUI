#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/EMLResource.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/DraggableView2D.hpp>
#include <Essa/Geometry/Line2D.hpp>
#include <Essa/Geometry/LineSegment2D.hpp>

namespace GeometryRenderers {

void draw(Gfx::Painter& painter, Eg::LineSegment2D const& line) {
    auto const& p1 = line.point1();
    auto const& p2 = line.point2();
    painter.draw_line({ { p1.x(), p1.y() }, { p2.x(), p2.y() } }, Gfx::LineDrawOptions { .color = Util::Colors::DarkCyan });
}

void draw(Gfx::Painter& painter, Eg::Line2D const& line, Util::Rectf const& visible_area, Util::Color color = Util::Colors::DarkCyan) {
    if (line.is_vertical() || line.dydx() > 1) {
        auto const& x0 = line.solve_for_x(visible_area.top);
        auto const& x1 = line.solve_for_x(visible_area.bottom_right().y());
        painter.draw_line(
            { { x0, visible_area.top }, { x1, visible_area.bottom_right().y() } }, Gfx::LineDrawOptions { .color = color }
        );
    }
    else {
        auto const& y0 = line.solve_for_y(visible_area.left);
        auto const& y1 = line.solve_for_y(visible_area.bottom_right().x());
        painter.draw_line(
            { { visible_area.left, y0 }, { visible_area.bottom_right().x(), y1 } }, Gfx::LineDrawOptions { .color = color }
        );
    }
}

}

class PlotView : public GUI::DraggableView2D {
private:
    virtual void draw(Gfx::Painter& painter) const override {
        Gfx::PainterTransformScope scope(painter, transform());
        
        // axes
        auto x_axis = Eg::Line2D::horizontal(0);
        auto y_axis = Eg::Line2D::vertical(0);
        GeometryRenderers::draw(painter, x_axis, this->visible_area(), Util::Color(0x800000ff));
        GeometryRenderers::draw(painter, y_axis, this->visible_area(), Util::Color(0x008000ff));

        // some example objects
        GeometryRenderers::draw(painter, Eg::LineSegment2D({ 0, 0 }, { 100, 100 }));
        GeometryRenderers::draw(painter, Eg::Line2D::from_axbyc({ 1, 1, 0 }), this->visible_area());
    }
};

EML_REGISTER_CLASS(PlotView)

class MainWidget : public GUI::Container {
public:
    virtual void on_init() override { (void)load_from_eml_resource(resource_manager().require<EML::EMLResource>("MainWidget.eml")); }
};

int main() {
    GUI::SimpleApplication<MainWidget> app("Geometry Playground");
    app.run();
    return 0;
}
