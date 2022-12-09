#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/DraggableView2D.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <EssaUtil/Length.hpp>

class MyView : public GUI::DraggableView2D {
public:
    void set_stats_label(GUI::Textfield& tf) {
        m_stats_label = &tf;
    }

private:
    virtual void draw(Gfx::Painter& painter) const override {
        Gfx::PainterTransformScope scope { painter, transform() };

        Gfx::RectangleDrawOptions rect;
        rect.fill_color = Util::Colors::DarkRed;
        painter.draw_rectangle({ -10, -10, 20, 20 }, rect);

        rect.fill_color = Util::Colors::Green;
        painter.draw_rectangle({ -110, -110, 20, 20 }, rect);

        rect.fill_color = Util::Colors::Blue;
        painter.draw_rectangle({ 90, 90, 20, 20 }, rect);

        m_stats_label->set_content(Util::UString { fmt::format("Offset={} Zoom={} VisibleArea=({}, {})",
            fmt::streamed(offset()), zoom(), fmt::streamed(visible_area().position()), fmt::streamed(visible_area().size())) });
    }

    GUI::Textfield* m_stats_label {};
};

int main() {
    GUI::Application app;
    auto& window = app.create_host_window({ 500, 500 }, "DraggableView2D");

    auto& container = window.set_main_widget<GUI::Container>();
    container.set_layout<GUI::BasicLayout>();

    auto view = container.add_widget<MyView>();
    view->set_size({ 100.0_perc, 100.0_perc });

    auto stats_label = container.add_widget<GUI::Textfield>();
    stats_label->set_size({ 100.0_perc, 32.0_px });
    stats_label->set_horizontal_alignment(GUI::Widget::Alignment::Center);
    stats_label->set_alignment(GUI::Align::Center);
    view->set_stats_label(*stats_label);

    app.run();
    return 0;
}
