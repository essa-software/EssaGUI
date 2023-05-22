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
    virtual void draw(RichTextContext const&, Util::Cs::Point2f position, Gfx::Painter&) const = 0;
    virtual std::unique_ptr<Base> clone() const = 0;
};

class Text : public Base {
public:
    explicit Text(Util::UString str, Util::Color const& color)
        : m_string(std::move(str))
        , m_color(color) { }

    virtual float wanted_size(RichTextContext const&) const override;
    virtual void draw(RichTextContext const&, Util::Cs::Point2f position, Gfx::Painter&) const override;
    virtual std::unique_ptr<Base> clone() const override { return std::make_unique<Text>(*this); }

private:
    Gfx::Text text(RichTextContext const&) const;

    Util::UString m_string;
    Util::Color m_color;
};

class LineBreak : public Base {
    virtual float wanted_size(RichTextContext const&) const override { return 0; }
    virtual void draw(RichTextContext const&, Util::Cs::Point2f, Gfx::Painter&) const override { }
    virtual std::unique_ptr<Base> clone() const override { return std::make_unique<LineBreak>(*this); }
};

class Image : public Base {
public:
    explicit Image(llgl::Texture const& tex)
        : m_texture(tex) { }

private:
    virtual float wanted_size(RichTextContext const&) const override;
    virtual void draw(RichTextContext const&, Util::Cs::Point2f position, Gfx::Painter&) const override;
    virtual std::unique_ptr<Base> clone() const override { return std::make_unique<Image>(*this); }

    Util::Cs::Size2f scaled_image_size(RichTextContext const&) const;

    llgl::Texture const& m_texture;
};

}
