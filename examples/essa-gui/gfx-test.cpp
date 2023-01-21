#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Drawing/Ellipse.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>

int main() {
    GUI::Window wnd { { 1000, 700 }, "Gfx test" };

    llgl::TTFFont fixed_font = llgl::TTFFont::open_from_file("../assets/fonts/SourceCodePro-Regular.otf");
    llgl::TTFFont regular_font = llgl::TTFFont::open_from_file("../assets/fonts/Xolonium-pn4D.ttf");

    Gfx::Painter painter { wnd.renderer() };
    while (true) {
        painter.reset();
        while (wnd.poll_event()) {
        }

        painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { wnd.rect() } }, wnd.rect()));

        wnd.clear(Util::Color { 100, 100, 100 });

        painter.draw(Gfx::Drawing::Rectangle {
            { 190, 30, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, 5,
                { 0, 0, 30, 0 }),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 320, 30, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Transparent, 0,
                { 0, 0, 30, 0 }),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 450, 30, 100, 100 },
            Gfx::Drawing::Fill::none(),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, 5,
                { 0, 0, 30, 0 }),
        });
        painter.draw(Gfx::Drawing::Rectangle {
            { 580, 30, 100, 100 },
            Gfx::Drawing::Fill::solid(Util::Colors::Lime),
            Gfx::Drawing::Outline::rounded(Util::Colors::Red, -5,
                { 0, 0, 30, 0 }),
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

        painter.render();
        wnd.display();
    }

    return 0;
}
