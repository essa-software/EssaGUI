#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Drawing/Fill.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <EssaUtil/ScopeGuard.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class MainWidget : public GUI::Widget {
private:
    virtual void draw(Gfx::Painter& painter) const override {
        painter.draw(Gfx::Drawing::Rectangle { { 10, 10, 50, 50 },
            Gfx::Drawing::Fill::solid(Util::Colors::Red) });
    }

    mutable llgl::Texture m_texture;
};

int main() {
    GUI::Application app;
    auto& window = app.create_host_window({ 700, 400 }, "TEST",
        { .flags = llgl::WindowFlags::Maximized | llgl::WindowFlags::Resizable | llgl::WindowFlags::TransparentBackground });

    window.set_background_color(Util::Colors::Transparent);
    window.set_main_widget<MainWidget>();

    app.run();
    return 0;
}
