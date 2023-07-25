#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EventLoop.hpp>
#include <Essa/GUI/Widgets/Checkbox.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

class MainWidget : public GUI::Container {
public:
    virtual void on_init() override {
        set_layout<GUI::VerticalBoxLayout>();

        // Read<T> from function
        auto* fps = add_widget<GUI::Textfield>();
        fps->content() = [&]() { return Util::UString::format("fps = {}", GUI::EventLoop::current().tps()); };
        fps->set_size({ 100_perc, 32_px });

        // Read<T> from CalculatedObservable
        auto* textbox = add_widget<GUI::Textbox>();
        textbox->set_type(GUI::Textbox::Type::TEXT);
        auto* textbox_content = add_widget<GUI::Textfield>();
        textbox_content->set_size({ 100_perc, 32_px });
        textbox_content->content() = textbox->content_observable();

        // Read<T> from Map
        auto* textbox_content_mapped = add_widget<GUI::Textfield>();
        textbox_content_mapped->set_size({ 100_perc, 32_px });
        textbox_content_mapped->content()
            = textbox->content_observable().map([](auto const& value) { return Util::UString::format("<<{}>>", value.encode()); });

        // Read<T> from mapped Observable
        auto* checkbox = add_widget<GUI::Checkbox>();
        checkbox->set_size({ 100_perc, 32_px });
        checkbox->set_caption("Observable");
        auto* checkbox_checked = add_widget<GUI::Textfield>();
        checkbox_checked->set_size({ 100_perc, 32_px });
        checkbox_checked->content()
            = checkbox->active().map([](bool value) { return Util::UString::format("Checkbox checked? {}", value); });

        // Independently assigning Observables
        auto* check_btn = add_widget<GUI::TextButton>();
        check_btn->set_content("Check checkbox");
        check_btn->on_click = [checkbox]() { checkbox->set_active(true); };
    }
};

int main() {
    GUI::SimpleApplication<MainWidget> app { "Reactivity", { 1000, 1000 } };
    app.run();
    return 0;
}
