#pragma once

#include <Essa/GUI/Widgets/Widget.hpp>
#include <Essa/LLGL/Core/Camera.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>

namespace GUI {

class WorldView;

// This class ensures that everything in the scope will be drawn
// with depth enabled. It shoud be used when drawing actual 3D
// things. Note that it also handles actual setting of Painter's
// viewport, which isn't done by ClipViewScope. 
class WorldDrawScope {
public:
    // Can be used for doing multiple layers.
    enum class ClearDepth {
        Yes,
        No
    };

    explicit WorldDrawScope(Gfx::Painter const& view, ClearDepth = ClearDepth::No);
    ~WorldDrawScope();

    static void verify();
    static WorldDrawScope const* current();

private:
    static WorldDrawScope const* s_current;

    WorldDrawScope const* m_parent = nullptr;
    llgl::Projection m_previous_projection;
};

class WorldView : public Widget {
private:
    friend WorldDrawScope;

    virtual void draw(Gfx::Painter&) const = 0;
};

}
