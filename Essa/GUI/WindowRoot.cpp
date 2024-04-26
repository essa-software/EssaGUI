#include "WindowRoot.hpp"

#include <Essa/GUI/Debug.hpp>
#include <Essa/GUI/EML/AST.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>

namespace GUI {

EML::EMLErrorOr<void> WindowRoot::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    auto title = TRY(object.get_property("title", EML::Value(Util::UString {})).to_string());
    auto width = TRY(object.get_property("width", EML::Value(0.0)).to_double());
    auto height = TRY(object.get_property("height", EML::Value(0.0)).to_double());
    auto center_on_screen = TRY(object.get_property("center_on_screen", EML::Value(false)).to_bool());
    llgl::WindowSettings settings;
    settings.flags = llgl::WindowFlags(0);

#define FLAG(name, Name, default)                                                  \
    if (TRY(object.get_property("flags_" #name, EML::Value(default)).to_bool())) { \
        settings.flags |= llgl::WindowFlags::Name;                                 \
    }

    // TODO: Better window flags
    FLAG(fullscreen, Fullscreen, false)
    FLAG(borderless, Borderless, false)
    FLAG(resizable, Resizable, true)
    FLAG(minimized, Minimized, false)
    FLAG(maximized, Maximized, false)
    FLAG(transparent_background, TransparentBackground, false)
    FLAG(shaped, Shaped, false)
    FLAG(tooltip, Tooltip, false)

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
        m_main_widget->set_size({ { static_cast<float>(m_window.size().x()), Util::Length::Px },
                                  { static_cast<float>(m_window.size().y()), Util::Length::Px } });
        m_main_widget->set_raw_size(m_window.size());
        m_main_widget->do_relayout();
        if (DBG_ENABLED(GUI_DumpLayout)) {
            fmt::print(stderr, "Window: {} (pos={}, size={})\n", typeid(m_window).name(), m_window.host_position(), m_window.size());
            fmt::print(stderr, "    Screen: {}\n", typeid(*this).name());
            m_main_widget->dump(std::cerr, 2);
        }
        m_needs_relayout = false;
    }
    m_main_widget->do_draw(painter);
}

void WindowRoot::do_handle_event(llgl::Event const& event) {
    if (!m_main_widget)
        return;

    if (event.is<llgl::Event::WindowResize>()) {
        m_needs_relayout = true;
    }
    if (event.is<llgl::Event::WindowClose>()) {
        fmt::print("close()\n");
        close();
    }

    if (handle_event(event) == Widget::EventHandlerResult::Accepted) {
        return;
    }

    // FIXME: Find a way to make first focusable widget focused "from start".
    if (!m_focused_widget && event.is<llgl::Event::KeyPress>()) {
        auto key_event = *event.get<llgl::Event::KeyPress>();
        if (key_event.code() == llgl::KeyCode::Tab) {
            m_main_widget->focus_first_child_or_self();
            return;
        }
    }
    if (auto const* mousemove = event.get<llgl::Event::MouseMove>()) {
        llgl::Cursor::set(main_widget()->cursor(mousemove->local_position()));
    }
    event.visit(
        [&](llgl::Event::WindowMouseEnter const&) { m_main_widget->do_handle_event(GUI::Event::MouseEnter()); },
        [&](llgl::Event::WindowMouseLeave const&) { m_main_widget->do_handle_event(GUI::Event::MouseLeave()); },
        [&](auto const& event) {
            if constexpr (requires() { GUI::Event::Variant(event); }) {
                m_main_widget->do_handle_event(GUI::Event(event));
            }
        }
    );
}

void WindowRoot::close() { m_window.close(); }

std::vector<DevToolsObject const*> WindowRoot::dev_tools_children() const {
    if (!m_main_widget) {
        return {};
    }
    return { m_main_widget.get() };
}

}
