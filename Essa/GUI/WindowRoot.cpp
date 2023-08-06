#include "WindowRoot.hpp"

#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>

namespace GUI {

EML::EMLErrorOr<void> WindowRoot::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    auto title = TRY(object.get_property("title", EML::Value(Util::UString {})).to_string());
    auto width = TRY(object.get_property("width", EML::Value(0.0)).to_double());
    auto height = TRY(object.get_property("height", EML::Value(0.0)).to_double());
    auto center_on_screen = TRY(object.get_property("center_on_screen", EML::Value(false)).to_bool());
    llgl::WindowSettings settings;

#define FLAG(name, Name)                                                         \
    if (TRY(object.get_property("flags_" #name, EML::Value(false)).to_bool())) { \
        settings.flags |= llgl::WindowFlags::Name;                               \
    }

    // TODO: Better window flags
    FLAG(fullscreen, Fullscreen)
    FLAG(borderless, Borderless)
    FLAG(resizable, Resizable)
    FLAG(minimized, Minimized)
    FLAG(maximized, Maximized)
    FLAG(transparent_background, TransparentBackground)
    FLAG(shaped, Shaped)
    FLAG(tooltip, Tooltip)

#undef FLAG

    m_window.setup(title, { width, height }, settings);
    if (center_on_screen) {
        m_window.center_on_screen();
    }

    set_created_main_widget(TRY(object.require_and_construct_object<Widget>("main_widget", loader, *this)));
    return {};
}

DBG_DECLARE(GUI_DumpLayout);

void WindowRoot::relayout_and_draw(Gfx::Painter& painter) {
    if (!m_main_widget)
        return;
    if (m_needs_relayout) {
        // std::cout << m_id << "\n"
        m_main_widget->set_size({ { static_cast<int>(m_window.size().x()), Util::Length::Px },
                                  { static_cast<int>(m_window.size().y()), Util::Length::Px } });
        m_main_widget->set_raw_size(m_window.size());
        m_main_widget->do_relayout();
        if (DBG_ENABLED(GUI_DumpLayout)) {
            fmt::print("Window: {} (pos={}, size={})\n", typeid(m_window).name(), m_window.position(), m_window.size());
            fmt::print("    Screen: {}\n", typeid(*this).name());
            m_main_widget->dump(2);
        }
        m_needs_relayout = false;
    }
    m_main_widget->do_draw(painter);
}

void WindowRoot::do_handle_event(GUI::Event const& event) {
    if (!m_main_widget)
        return;

    if (event.is<GUI::Event::WindowResize>()) {
        m_needs_relayout = true;
    }
    if (event.is<GUI::Event::WindowClose>()) {
        fmt::print("close()\n");
        close();
    }

    if (handle_event(event) == Widget::EventHandlerResult::Accepted) {
        return;
    }

    // FIXME: Find a way to make first focusable widget focused "from start".
    if (!m_focused_widget && event.is<GUI::Event::KeyPress>()) {
        auto key_event = *event.get<GUI::Event::KeyPress>();
        if (key_event.code() == llgl::KeyCode::Tab) {
            m_main_widget->focus_first_child_or_self();
            return;
        }
    }
    m_main_widget->do_handle_event(event);
}

void WindowRoot::close() { m_window.close(); }

}
