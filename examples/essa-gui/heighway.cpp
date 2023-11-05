#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/LLGL/OpenGL/Framebuffer.hpp>
#include <EssaUtil/Clock.hpp>
#include <EssaUtil/Random.hpp>

using namespace std::chrono_literals;

class MainWidget : public GUI::Widget {
private:
    virtual void update() override {
        constexpr int PointsPerTick = 100000;
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
        painter.deprecated_draw_rectangle(local_rect().cast<float>(), rect);

        Gfx::Text text {
            Util::UString::format(
                "{}; {:.2f} FPS; {} pps", m_point_count, GUI::Application::the().tps(),
                std::floor(static_cast<double>(m_point_count) / (m_clock.elapsed() / 1.0s))
            ),
            resource_manager().font(),
        };
        text.set_font_size(16);
        text.set_fill_color(Util::Colors::Wheat);
        text.set_position({ 10, 20 });
        text.draw(painter);
    }

    void append_point() {
        bool equation = Util::Random::default_engine().next_bool(2);
        auto old_position = m_current_position;
        if (equation) {
            m_current_position.set_x(-0.4 * old_position.x() - 1);
            m_current_position.set_y(-0.4 * old_position.y() + 0.1);
        }
        else {
            m_current_position.set_x(0.76 * old_position.x() - 0.4 * old_position.y());
            m_current_position.set_y(0.4 * old_position.x() + 0.76 * old_position.y());
        }

        m_painter.set_projection(llgl::Projection { {}, { 0, 0, 500, 500 } });
        Gfx::RectangleDrawOptions point;
        point.fill_color = Util::Colors::White;
        m_painter.deprecated_draw_rectangle({ m_current_position, { 0.001, 0.001 } }, point);
    }

    llgl::Framebuffer m_framebuffer { { 500, 500 } };
    Gfx::Painter m_painter { m_framebuffer.renderer() };
    Util::Point2f m_current_position;
    uint64_t m_point_count = 0;
    Util::Clock m_clock;
};

int main() {
    GUI::Application app;
    auto& wnd = app.create_host_window({ 750, 750 }, "Heighway's Dragon (Gfx::Painter benchmark)");

    wnd.set_root_widget<MainWidget>();

    app.run();
    return 0;
}
