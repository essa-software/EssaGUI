#pragma once

#include "Context.hpp"
#include <Essa/GUI/Graphics/Painter.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>

namespace Gfx::RichTextFragments {

class Base {
public:
    virtual ~Base() = default;
    virtual float wanted_size(RichTextContext const&) const = 0;
    virtual void draw(RichTextContext const&, Util::Vector2f position, Gfx::Painter&) const = 0;
};

class Text : public Base {
public:
    explicit Text(Util::UString str, Util::Color const& color)
        : m_string(std::move(str))
        , m_color(color) { }

    virtual float wanted_size(RichTextContext const&) const override;
    virtual void draw(RichTextContext const&, Util::Vector2f position, Gfx::Painter&) const override;

private:
    Gfx::Text text(RichTextContext const&) const;

    Util::UString m_string;
    Util::Color m_color;
};

class LineBreak : public Base {
    virtual float wanted_size(RichTextContext const&) const override { return 0; }
    virtual void draw(RichTextContext const&, Util::Vector2f, Gfx::Painter&) const override { }
};
}
