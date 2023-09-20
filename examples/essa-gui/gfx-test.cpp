#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Drawing/Ellipse.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>

int main() {
    llgl::Window wnd { { 1000, 700 }, "Gfx test" };

    llgl::TTFFont fixed_font = llgl::TTFFont::open_from_file("../assets/fonts/SourceCodePro-Regular.otf");
    llgl::TTFFont regular_font = llgl::TTFFont::open_from_file("../assets/fonts/Xolonium-pn4D.ttf");

    Gfx::Painter painter { wnd.renderer() };
    while (true) {
        painter.reset();
        while (wnd.poll_event()) { }

        painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { wnd.rect() } }, wnd.rect()));

        wnd.renderer().clear(Util::Color { 100, 100, 100 });

        painter.draw(Gfx::Drawing::Rectangle {
            { 190, 30, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, 5, { 0, 0, 30, 0 }),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 320, 30, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Transparent, 0, { 0, 0, 30, 0 }),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 450, 30, 100, 100 },
            Gfx::Drawing::Fill::none(),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, 5, { 0, 0, 30, 0 }),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 580, 30, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, -5, { 0, 0, 30, 0 }),
        });

        painter.draw(Gfx::Drawing::Rectangle {
            { 190, 160, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, 5, 100),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 320, 160, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Transparent, 0, 100),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 450, 160, 100, 100 },
            Gfx::Drawing::Fill::none(),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, 5, 100),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 580, 160, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, -5, 100),
        });

        painter.draw(Gfx::Drawing::Ellipse {
            { 240, 350 },
            { 50, 70 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::normal(Util::Colors::Red, 5),
        }
                         .set_point_count(6));
        painter.draw(Gfx::Drawing::Ellipse {
            { 370, 350 },
            { 50, 70 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Transparent, 0, 20),
        }
                         .set_point_count(6));
        painter.draw(Gfx::Drawing::Ellipse {
            { 500, 350 },
            { 50, 70 },
            Gfx::Drawing::Fill::none(),
            Gfx::Drawing::Outline::normal(Util::Colors::Red, 5),
        }
                         .set_point_count(6));
        painter.draw(Gfx::Drawing::Ellipse {
            { 630, 350 },
            { 50, 70 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, -5, 20),
        }
                         .set_point_count(6));

        char ascii[] = R"~~~( !"#$%&'()*+,-./
0123456789:;<=>?
@ABCDEFGHIJKLMNO
PQRSTUVWXYZ[\]^_
`abcdefghijklmno
pqrstuvwxyz{|}~ 
---------------
 ¡¢£¤¥¦§¨©ª«¬ ®¯
°±²³´µ¶·¸¹º»¼½¾¿
ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ
ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß
àáâãäåæçèéêëìíîï
ðñòóôõö÷øùúûüýþÿ)~~~";

        Gfx::Text text { ascii, fixed_font };
        text.set_position({ 700, 40 });
        text.set_font_size(30);
        text.draw(painter);

        painter.draw_line({ 10, 10 }, { 80, 100 }, { .color = Util::Colors::Red });
        painter.draw_line({ { 150, 10 }, { 110, 40 }, { 150, 70 }, { 110, 100 } }, { .color = Util::Colors::Lime });

        {
            // Rounding test
            Gfx::Text lorem { "Create Object | Simulate | Radius | Mass", regular_font };
            lorem.set_font_size(15);
            lorem.set_position({ 10, 550 });
            lorem.draw(painter);
            lorem.set_position({ 20.5, 580 });
            lorem.draw(painter);
            lorem.set_position({ 30, 610.5 });
            lorem.draw(painter);
            lorem.set_position({ 40.5, 640.5 });
            lorem.draw(painter);
        }

        // Shape border rounding edge cases
        class GenericShape : public Gfx::Drawing::Shape {
        public:
            GenericShape(std::vector<Util::Point2f> points)
                : Shape(Gfx::Drawing::Fill::none(), Gfx::Drawing::Outline::rounded(Util::Colors::Red, 2, 5))
                , m_points(std::move(points)) { }

            virtual size_t point_count() const { return m_points.size(); }
            virtual Util::Point2f point(size_t idx) const { return m_points[idx]; }

            // Bounds that are used for calculating texture rect. That is,
            // point at [size()] will use bottom right corner of texture rect.
            virtual Util::Rectf local_bounds() const { return { 0, 0, 1, 1 }; }

        private:
            std::vector<Util::Point2f> m_points;
        };

        // All these should render an identical triangle
        painter.draw(GenericShape({ { 100, 0 }, { 0, 0 }, { 0, 0 }, { 0, 70 } }) //
                         .set_transform(llgl::Transform().translate_2d({ 190, 450 })));
        painter.draw(GenericShape({ { 0, 0 }, { 100, 0 }, { 0, 70 }, { 0, 0 } }) //
                         .set_transform(llgl::Transform().translate_2d({ 320, 450 })));
        painter.draw(GenericShape({ { 0, 70 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } }) //
                         .set_transform(llgl::Transform().translate_2d({ 450, 450 })));
        painter.draw(GenericShape({ { 0, 0 }, { 0, 0 }, { 0, 70 }, { 100, 0 }, { 0, 0 } }) //
                         .set_transform(llgl::Transform().translate_2d({ 580, 450 })));

        painter.render();
        wnd.display();
    }

    return 0;
}
