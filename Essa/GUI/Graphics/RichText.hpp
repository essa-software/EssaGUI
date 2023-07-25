#pragma once

#include "Painter.hpp"
#include "RichText/Context.hpp"
#include "RichText/Fragments.hpp"
#include <EssaUtil/NonCopyable.hpp>
#include <memory>

namespace Gfx {

class RichText {
public:
    RichText() = default;
    RichText(RichText const&);
    RichText& operator=(RichText const&);
    RichText(RichText&&) = default;
    RichText& operator=(RichText&&) = default;

    template<class T, class... Args>
        requires(std::is_base_of_v<RichTextFragments::Base, T>)
    RichText& append_fragment(Args&&... args) {
        m_fragments.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *this;
    }

    RichText& append(Util::UString const&, Util::Color const&);
    RichText& append_image(llgl::Texture const&);

    std::vector<std::unique_ptr<RichTextFragments::Base>> const& fragments() const { return m_fragments; }

private:
    std::vector<std::unique_ptr<RichTextFragments::Base>> m_fragments;
};

class RichTextDrawable {
public:
    RichTextDrawable(RichText const& text, RichTextContext context)
        : m_text(text)
        , m_context(std::move(context)) { }

    void set_rect(Util::Rectf rect) { m_rect = rect; }
    void draw(Gfx::Painter& painter) const;

private:
    Util::Rectf m_rect;
    RichText const& m_text;
    RichTextContext m_context;
};

}
