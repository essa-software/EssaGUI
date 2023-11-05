#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Drawing/Fill.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/LLGL/OpenGL/Framebuffer.hpp>
#include <EssaUtil/Clock.hpp>
#include <EssaUtil/Random.hpp>

using namespace std::chrono_literals;

constexpr float Size = 750;

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
        rect.texture_rect = { 0, 0, static_cast<unsigned int>(Size), static_cast<unsigned int>(Size) };
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
        bool equation = Util::Random::default_engine().next_bool(5);
        auto old_position = m_current_position;
        if (equation) {
            m_current_position.set_x(-0.4 * old_position.x() - 1);
            m_current_position.set_y(-0.4 * old_position.y() + 0.1);
        }
        else {
            m_current_position.set_x(0.76 * old_position.x() - 0.4 * old_position.y());
            m_current_position.set_y(0.4 * old_position.x() + 0.76 * old_position.y());
        }

        m_painter.set_projection(llgl::Projection { {}, { 0, 0, static_cast<int>(Size), static_cast<int>(Size) } });

        m_rectangle.set_transform(llgl::Transform());
        m_painter.draw(m_rectangle.move(m_current_position.to_vector()));
    }

    Gfx::Drawing::Rectangle m_rectangle { { 1 / Size, 1 / Size }, Gfx::Drawing::Fill::solid(Util::Colors::White) };
    llgl::Framebuffer m_framebuffer { { Size, Size } };
    Gfx::Painter m_painter { m_framebuffer.renderer() };
    Util::Point2f m_current_position {};
    uint64_t m_point_count = 0;
    Util::Clock m_clock;
};

int main() {
    GUI::Application app;
    auto& wnd = app.create_host_window({ Size, Size }, "Heighway's Dragon (Gfx::Painter benchmark)");
    wnd.set_root_widget<MainWidget>();

    app.run();
    return 0;
}
