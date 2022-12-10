#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>

int main() {
    GUI::Window wnd { { 1000, 1000 }, "Gfx test" };

    llgl::TTFFont fixed_font = llgl::TTFFont::open_from_file("../assets/fonts/SourceCodePro-Regular.otf");
    llgl::TTFFont regular_font = llgl::TTFFont::open_from_file("../assets/fonts/Xolonium-pn4D.ttf");

    Gfx::Painter painter { wnd.renderer() };
    while (true) {
        painter.reset();
        llgl::Event e;
        while (wnd.poll_event(e)) {
        }

        painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { wnd.rect() } }, wnd.rect()));

        wnd.clear(Util::Color { 100, 100, 100 });
        Gfx::Text text { "/home/sppmacd #T3$%abcdefghijklmnopqurtsdhstrhstyjstr6", fixed_font };
        text.set_position({ 50, 200 });
        text.set_font_size(75);
        text.draw(painter);

        painter.draw_line({ 10, 10 }, { 80, 100 }, { .color = Util::Colors::Red });
        painter.draw_line({ { 150, 10 }, { 110, 40 }, { 150, 70 }, { 110, 100 } }, { .color = Util::Colors::Lime });

        {
            // Rounding test
            Gfx::Text lorem { "Create Object | Simulate | Radius | Mass", regular_font };
            lorem.set_font_size(15);
            lorem.set_position({ 10, 300 });
            lorem.draw(painter);
            lorem.set_position({ 20.5, 330 });
            lorem.draw(painter);
            lorem.set_position({ 30, 360.5 });
            lorem.draw(painter);
            lorem.set_position({ 40.5, 390.5 });
            lorem.draw(painter);
        }

        painter.render();
        wnd.display();
    }

    return 0;
}
