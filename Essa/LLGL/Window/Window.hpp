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
class SDLWindowData;
}

class Window {
public:
    Window(Util::Vector2i size, Util::UString const& title, WindowSettings const& = {});
    ~Window();

    Window(Window&&) = default;
    Window& operator=(Window&&) = default;

    static Window create_foreign(std::unique_ptr<Detail::SDLWindowData>);

    void create(Util::Vector2i size, Util::UString const& title, WindowSettings const& = {});

    void close();
    void set_title(Util::UString const&);
    Util::UString title() const;
    void set_size(Util::Vector2i);
    void set_position(Util::Vector2i);
    void display();
    std::optional<Event> poll_event();
    void set_mouse_position(Util::Vector2i);
    bool is_focused() const;
    void set_active() const;
    void maximize() const;
    Util::Recti system_rect() const;

    Renderer& renderer() { return m_renderer; }
    Util::Vector2i size() const { return m_size; }
    Util::Vector2f center() const { return Util::Vector2f { m_size } / 2.f; }
    float aspect() const { return (float)m_size.x() / m_size.y(); }
    Util::Recti rect() const { return { 0, 0, size().x(), size().y() }; }

    // Get a size of a screen the window is currently on.
    Util::Vector2i screen_size() const;

    void center_on_screen();

    Detail::SDLWindowData* window_data() { return m_data.get(); }

private:
    Window() = default;

    void create_impl(Util::Vector2i size, Util::UString const& title, WindowSettings const& = {});
    void set_size_impl(Util::Vector2i);
    std::optional<Event> poll_event_impl();
    void destroy();

    // Should we care about removing the window?
    bool m_foreign = false;

    // Don't require user to include all of SDL
    std::unique_ptr<Detail::SDLWindowData> m_data;
    Renderer m_renderer { 0 };
    Util::Vector2i m_size;
};

}
