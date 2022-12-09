#include <EssaEngine/3D/Shaders/Basic.hpp>
#include <EssaGUI/Graphics/GUIBuilder.hpp>
#include <EssaGUI/Graphics/Vertex.hpp>
#include <EssaGUI/Application.hpp>
#include <EssaGUI/Widgets/WorldView.hpp>
#include <LLGL/Core/Transform.hpp>
#include <LLGL/OpenGL/PrimitiveType.hpp>
#include <fmt/ostream.h>

class WorldView : public GUI::WorldView {
private:
    virtual void draw(Gfx::Painter& window) const override {
        Gfx::GUIBuilder builder;
        builder.set_projection(llgl::Projection::ortho({ Util::Rectd { 0, 0, 220 * raw_size().x() / raw_size().y(), 220 } }, Util::Recti { rect() }));
        builder.add_rectangle({ 25, 25, 50, 50 }, { .fill_color = Util::Colors::Red });
        builder.add_rectangle({ 145, 145, 50, 50 }, { .fill_color = Util::Colors::Green });
        builder.add_regular_polygon({ 110, 110 }, 25, 5, Util::Colors::LightBlue);
        builder.add_regular_polygon({ 50, 160 }, 25, 3, Util::Colors::LightYellow);
        builder.add_regular_polygon({ 160, 50 }, 25, 30, Util::Colors::DarkSlateGray);
        builder.render(window.renderer());
    }
};

using Vert = llgl::Vertex<Util::Vector2f, Util::Colorf, Util::Vector2f>;
template<>
struct llgl::VertexMapping<Vert> {
    static constexpr size_t position = 0;
    static constexpr size_t color = 1;
    static constexpr size_t tex_coord = 2;
};

int main() {
    GUI::Application app;
    auto& window = app.create_host_window({ 440, 440 }, "VAO Builder example");
    window.set_main_widget<WorldView>();
    app.run();
    return 0;
}
