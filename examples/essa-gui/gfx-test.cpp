#include <EssaGUI/gfx/Text.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/FileExplorer.hpp>
#include <LLGL/Resources/TTFFont.hpp>

int main() {
    GUI::Window wnd { { 1000, 1000 }, "Gfx test" };

    llgl::TTFFont font = llgl::TTFFont::open_from_file("../assets/fonts/SourceCodePro-Regular.otf");

    while (true) {
        llgl::Event e;
        while (wnd.poll_event(e)) {
        }
        wnd.clear(Util::Color { 100, 100, 100 });
        Gfx::Text text { "/home/sppmacd #T3$%abcdefghijklmnopqurtsdhstrhstyjstr6", font };
        text.set_position({ 50, 200 });
        text.set_font_size(75);
        text.draw(wnd);
        wnd.display();
    }

    return 0;
}
