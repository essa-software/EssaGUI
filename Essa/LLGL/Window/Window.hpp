#pragma once

#include "ContextSettings.hpp"
#include "Event.hpp"
#include "Impls/WindowImpl.hpp"

#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Vector.hpp>

#include <memory>
#include <string>

namespace llgl {

class Window {
public:
    Window(Util::Vector2i size, Util::UString const& title, ContextSettings const& = {});

    // These are passed to WindowImpl
    void create(Util::Vector2i size, Util::UString const& title, ContextSettings const& = {});
    void close();
    void set_title(Util::UString const&);
    void set_size(Util::Vector2i);
    void set_position(Util::Vector2i);
    void display();
    bool poll_event(Event&);
    void set_mouse_position(Util::Vector2i);
    bool is_focused() const;
    void set_active() const;
    void maximize() const;

    Renderer& renderer() { return m_renderer; }
    Util::Vector2i size() const { return m_size; }
    Util::Vector2f center() const { return Util::Vector2f { m_size } / 2.f; }
    float aspect() const { return (float)m_size.x() / m_size.y(); }
    Util::Recti rect() const { return { 0, 0, size().x(), size().y() }; }

    void center_on_screen() const;

private:
    std::unique_ptr<WindowImpl> m_impl;
    Renderer m_renderer { 0 };
    Util::Vector2i m_size;
};

}
