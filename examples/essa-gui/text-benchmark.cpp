

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <EssaUtil/UStringBuilder.hpp>

constexpr size_t StringLength = 102400;

class App : public GUI::Widget {
public:
    App() {
        Util::UStringBuilder builder;
        for (size_t s = 0; s < StringLength; s++) {
            builder.append('a');
        }
        m_string = builder.release_string();
    }

private:
    virtual void draw(Gfx::Painter& painter) const override {
        Gfx::Text fps(Util::to_ustring(GUI::Application::the().tps()), resource_manager().font());
        fps.set_fill_color(Util::Colors::White);
        fps.align(GUI::Align::TopLeft, local_rect().cast<float>());
        fps.draw(painter);

        Gfx::Text text(m_string, resource_manager().font());
        text.set_fill_color(Util::Colors::White);
        text.set_position({ 0, 100 });
        text.draw(painter);
    }

    Util::UString m_string;
};

int main() {
    GUI::SimpleApplication<App> app("app");
    app.run();
    return 0;
}
