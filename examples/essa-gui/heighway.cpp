#include <EssaGUI/gfx/Painter.hpp>
#include <EssaGUI/gfx/Text.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaUtil/Clock.hpp>
#include <EssaUtil/Random.hpp>
#include <LLGL/OpenGL/Framebuffer.hpp>

using namespace std::chrono_literals;

class MainWidget : public GUI::Widget {
private:
    virtual void update() override {
        constexpr int PointsPerTick = 500000;
        m_painter.reset();
        for (int i = 0; i < PointsPerTick; i++) {
            append_point();
        }
        m_point_count += PointsPerTick;
        m_painter.render();
    }

    virtual void draw(Gfx::Painter& painter) const override {
        Gfx::RectangleDrawOptions rect;
        rect.texture = &m_framebuffer.color_texture();
        rect.texture_rect = { 0, 0, 500, 500 };
        painter.draw_rectangle(local_rect(), rect);

        Gfx::Text text { Util::UString { std::to_string(m_point_count) + "; "
                             + std::to_string(static_cast<int>(GUI::Application::the().tps())) + " FPS; "
                             + std::to_string(std::floor(static_cast<double>(m_point_count) / (m_clock.elapsed() / 1.0s))) + " pps" },
            resource_manager().font() };
        text.set_font_size(16);
        text.set_fill_color(Util::Colors::Wheat);
        text.set_position({ 10, 20 });
        text.draw(painter);
    }

    void append_point() {
        bool equation = Util::Random::default_engine()() % 2;
        auto old_position = m_current_position;
        if (equation) {
            m_current_position.x() = -0.4 * old_position.x() - 1;
            m_current_position.y() = -0.4 * old_position.y() + 0.1;
        }
        else {
            m_current_position.x() = 0.76 * old_position.x() - 0.4 * old_position.y();
            m_current_position.y() = 0.4 * old_position.x() + 0.76 * old_position.y();
        }

        m_painter.builder().set_projection(llgl::Projection { {}, { 0, 0, 500, 500 } });
        Gfx::RectangleDrawOptions point;
        point.fill_color = Util::Colors::White;
        m_painter.draw_rectangle({ m_current_position, { 0.001, 0.001 } }, point);
    }

    llgl::Framebuffer m_framebuffer { { 500, 500 } };
    Gfx::Painter m_painter { m_framebuffer.renderer() };
    Util::Vector2f m_current_position;
    uint64_t m_point_count = 0;
    Util::Clock m_clock;
};

int main() {
    GUI::Application app;
    auto& wnd = app.create_host_window({ 750, 750 }, "Heighway's Dragon (Gfx::Painter benchmark)");

    wnd.set_main_widget<MainWidget>();

    app.run();
    return 0;
}