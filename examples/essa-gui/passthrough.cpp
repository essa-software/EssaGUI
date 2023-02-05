#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Drawing/Fill.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/LLGL/Window/Impls/SDLWindow.hpp>
#include <EssaUtil/ScopeGuard.hpp>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_video.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shapeconst.h>

class MainWidget : public GUI::Widget {
private:
    virtual void draw(Gfx::Painter& painter) const override {
        painter.draw(Gfx::Drawing::Rectangle { rect(), Gfx::Drawing::Fill::solid(Util::Color { 0x00ff0040 }) });

        Gfx::Text text { "Use the command line you used to open this window to close it. There is no other way.", GUI::Application::the().font() };
        text.set_font_size(30);
        text.align(GUI::Align::Center, rect());
        text.draw(painter);
    }

    mutable llgl::Texture m_texture;
};

int main() {
    GUI::Application app;
    auto& window = app.create_host_window({ 700, 400 }, "TEST",
        { .flags = llgl::WindowFlags::Resizable | llgl::WindowFlags::TransparentBackground | llgl::WindowFlags::Borderless });
    window.set_size(window.screen_size());
    window.center_on_screen();

#ifdef SDL_VIDEO_DRIVER_X11
    auto sdl_window = window.window_data()->window;
    SDL_SetWindowAlwaysOnTop(sdl_window, SDL_TRUE);
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (!SDL_GetWindowWMInfo(sdl_window, &wminfo)) {
        fmt::print("SDL_GetWindowWMInfo: {}\n", SDL_GetError());
    }
    auto region = XFixesCreateRegion(wminfo.info.x11.display, nullptr, 0);
    XFixesSetWindowShapeRegion(wminfo.info.x11.display, wminfo.info.x11.window, ShapeInput, 0, 0, region);
#else
#    warning This works only on X11.
#endif

    window.set_background_color(Util::Colors::Transparent);
    window.set_main_widget<MainWidget>();

    app.run();
    return 0;
}
