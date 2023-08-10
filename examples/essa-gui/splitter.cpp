

#include "EssaUtil/Length.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Splitter.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

int main() {
    GUI::SimpleApplication<GUI::HorizontalSplitter> app("Splitters");

    auto* t1 = app.main_widget().add_widget<GUI::VerticalSplitter>();
    t1->set_size({ 100_px, Util::Length::Auto });

    auto* t11 = t1->add_widget<GUI::Textfield>();
    t11->set_content("Title 1");
    auto* t12 = t1->add_widget<GUI::Textfield>();
    t12->set_content("Title 2");
    t12->set_size({ Util::Length::Auto, Util::Length::Auto });

    auto* t2 = app.main_widget().add_widget<GUI::TextButton>();
    t2->set_content("Left Main Content");
    auto* t3 = app.main_widget().add_widget<GUI::TextButton>();
    t3->set_content("Right Main Content");

    app.run();
    return 0;
}
