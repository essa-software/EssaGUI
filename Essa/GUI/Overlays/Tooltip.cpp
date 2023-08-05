#include "Tooltip.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

namespace GUI {

TooltipOverlay::TooltipOverlay(WidgetTreeRoot& window, Tooltip t)
    : WindowRoot(window) {
    m_textfield = &set_main_widget<GUI::Textfield>();
    window.setup("TooltipOverlay", m_textfield->needed_size_for_text(), llgl::WindowSettings { .flags = llgl::WindowFlags::Tooltip });
    set_text(std::move(t.text));
    m_textfield->set_background_color(GUI::Application::the().theme().tooltip.background);
    // TODO: Tooltip color
}

void TooltipOverlay::set_text(Util::UString text) {
    m_text = text;
    m_textfield->set_content(std::move(text));
    window().set_size(m_textfield->needed_size_for_text().cast<int>());
}
Util::UString TooltipOverlay::text() const { return m_text; }

}
