#pragma once

#include "Event.hpp"
#include "WindowSettings.hpp"

#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <EssaUtil/Enum.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>

#include <memory>
#include <string>

namespace llgl {

namespace Detail {
struct SDLWindowData;
}

class Window {
public:
    Window();
    Window(Util::Size2u size, Util::UString const& title, WindowSettings const& = {});
    ~Window();

    Window(Window&&) = default;
    Window& operator=(Window&&) = default;

    static Window create_foreign(std::unique_ptr<Detail::SDLWindowData>);

    void create(Util::Size2u size, Util::UString const& title, WindowSettings const& = {});

    bool is_closed() const;
    void close();
    void set_title(Util::UString const&);
    Util::UString title() const;
    void set_size(Util::Size2u);
    void set_position(Util::Point2i);
    void display();
    std::optional<Event> poll_event();
    void set_mouse_position(Util::Point2i);
    bool is_focused() const;
    void set_active() const;
    void maximize();
    Util::Recti system_rect() const;

    Renderer& renderer() { return m_renderer; }
    Util::Point2i position() const;
    Util::Size2u size() const { return m_size; }
    Util::Point2u center() const { return (m_size / 2).to_vector().to_point(); }
    float aspect() const { return (float)m_size.x() / m_size.y(); }
    Util::Recti rect() const { return { 0, 0, static_cast<int>(size().x()), static_cast<int>(size().y()) }; }

    // Get a size of a screen the window is currently on.
    Util::Size2u screen_size() const;

    void center_on_screen();

    Detail::SDLWindowData* window_data() { return m_data.get(); }

private:
    void create_impl(Util::Size2u size, Util::UString const& title, WindowSettings const& = {});
    void set_size_impl(Util::Size2u);
    std::optional<Event> poll_event_impl();
    void destroy();

    // Should we care about removing the window?
    bool m_foreign = false;

    // Don't require user to include all of SDL
    std::unique_ptr<Detail::SDLWindowData> m_data;
    Renderer m_renderer { 0 };
    Util::Size2u m_size;
};

}
