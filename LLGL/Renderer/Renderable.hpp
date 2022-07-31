#pragma once

namespace llgl
{

class DrawState;
class Renderer;

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void render(Renderer&, DrawState) const = 0;
};

}
