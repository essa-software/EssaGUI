#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/RichText.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>
#include <Essa/LLGL/Resources/TTFFont.hpp>

class RichTextWidget : public GUI::Widget {
public:
    void set_content(Gfx::RichText text) { m_text = std::move(text); }

private:
    virtual void draw(Gfx::Painter& painter) const override {
        Gfx::RichTextDrawable drawable { m_text,
            {
                .default_font = GUI::Application::the().fixed_width_font(),
                .font_size = static_cast<int>(theme().label_font_size),
            } };
        drawable.set_rect(local_rect());
        drawable.draw(painter);
    }

    Gfx::RichText m_text;
};

int main() {
    GUI::Application app;
    auto& window = app.create_host_window({ 1000, 1000 }, "Rich Text");

    Gfx::RichText text;

    text.append("Lorem ipsum dolor sit amet consectetur adipisicing elit. Modi \
iure unde optio error laboriosam exercitationem nam temporibus? Minus corporis \
consectetur cumque non error. Harum totam, accusamus doloremque corporis rem obcaecati? ",
        Util::Colors::White);

    text.append("This thing will cost 50", Util::Colors::White);
    text.append_image(app.resource_manager().require_texture("gui/executableFile.png"));

    text.append("Lorem ipsum dolor\u00a0sit (NBSP working as expected) amet consectetur adipisicing elit. Modi \
iure unde optio error laboriosam exercitationem nam temporibus? Minus corporis \
consectetur cumque non error. Harum totam, accusamus doloremque corporis rem obcaecati? ",
        Util::Colors::Red);
    text.append("\n", Util::Colors::White);
    text.append("Lorem ipsum dolor sit amet consectetur adipisicing elit. Modi \
iure unde optio error laboriosam exercitationem nam temporibus? Minus corporis \
consectetur cumque non error. Harum totam, accusamus doloremque corporis rem obcaecati?",
        Util::Colors::Green);

    auto& widget = window.set_main_widget<RichTextWidget>();
    widget.set_content(std::move(text));

    app.run();
    return 0;
}
