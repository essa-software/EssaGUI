#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <fmt/ostream.h>

class TooltipHostWidget : public GUI::TextButton {
public:
    TooltipHostWidget(bool show_position = false)
        : m_show_position(show_position) {
        set_tooltip_text("test");
    }

private:
    virtual bool should_display_tooltip(
        Util::Cs::Point2i const& position) const override {
        return position.x() > raw_size().x() / 2;
    }
    virtual Util::UString create_tooltip(
        Util::Cs::Point2i const& position) const override {
        if (m_show_position)
            return Util::UString { fmt::format("{}", position) };
        return Widget::create_tooltip(position);
    }
    virtual void update_tooltip(Util::Cs::Point2i const& position,
        Util::UString& string) const override {
        if (m_show_position)
            string = Util::UString { fmt::format("{}", position) };
    }

    bool m_show_position = false;
};

int main() {
    GUI::Application app;

    auto& window = app.create_host_window({ 500, 500 }, "Tooltips");

    auto& container = window.set_main_widget<GUI::Container>();
    auto& layout = container.set_layout<GUI::VerticalBoxLayout>();
    layout.set_spacing(40);
    layout.set_padding(GUI::Boxi::all_equal(40));

    {
        auto layer1 = container.add_widget<GUI::Container>();
        auto& layer1_layout = layer1->set_layout<GUI::HorizontalBoxLayout>();
        layer1_layout.set_spacing(40);

        {
            auto host11 = layer1->add_widget<TooltipHostWidget>();
            host11->set_content("Hint");
            auto host12 = layer1->add_widget<TooltipHostWidget>();
            host12->set_content("Realtime");
            host12->set_tooltip_mode(GUI::Widget::TooltipMode::Realtime);
        }

        auto layer2 = container.add_widget<GUI::Container>();
        auto& layer2_layout = layer2->set_layout<GUI::HorizontalBoxLayout>();
        layer2_layout.set_spacing(40);

        {
            auto host21 = layer2->add_widget<TooltipHostWidget>(true);
            host21->set_tooltip_mode(GUI::Widget::TooltipMode::Realtime);
            host21->set_content("Realtime, updated");
            auto host22 = layer2->add_widget<TooltipHostWidget>();
            host22->set_content("Coming soon");
        }
    }

    app.run();
    return 0;
}
