#include <EssaGUI/Graphics/Text.hpp>
#include <EssaGUI/GUI/Application.hpp>
#include <EssaGUI/GUI/FileExplorer.hpp>
#include <LLGL/Resources/TTFFont.hpp>

int main() {
    GUI::Window wnd { { 1000, 1000 }, "Gfx test" };

    llgl::TTFFont font = llgl::TTFFont::open_from_file("../assets/fonts/SourceCodePro-Regular.otf");

    Gfx::Painter painter { wnd.renderer() };
    while (true) {
        painter.reset();
        llgl::Event e;
        while (wnd.poll_event(e)) {
        }

        painter.builder().set_projection(llgl::Projection::ortho({ Util::Rectd { wnd.rect() } }, wnd.rect()));

        wnd.clear(Util::Color { 100, 100, 100 });
        Gfx::Text text { "/home/sppmacd #T3$%abcdefghijklmnopqurtsdhstrhstyjstr6", font };
        text.set_position({ 50, 200 });
        text.set_font_size(75);
        text.draw(painter);

        painter.draw_line({ 10, 10 }, { 80, 100 }, { .color = Util::Colors::Red });
        painter.draw_line({ { 150, 10 }, { 110, 40 }, { 150, 70 }, { 110, 100 } }, { .color = Util::Colors::Lime });

        painter.render();
        wnd.display();
    }

    return 0;
}
