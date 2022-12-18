#pragma once

#include <Essa/GUI/Graphics/Painter.hpp>

namespace GUI {

class Theme;
class TextButton;

class ThemeRenderer {
public:
    explicit ThemeRenderer(GUI::Theme const& theme)
        : m_theme(theme) { }

    virtual ~ThemeRenderer() = default;

    virtual void draw_text_button_background(GUI::TextButton const&, Gfx::Painter&) const = 0;

protected:
    auto& theme() const { return m_theme; }

private:
    GUI::Theme const& m_theme;
};

class DefaultThemeRenderer : public ThemeRenderer {
public:
    explicit DefaultThemeRenderer(GUI::Theme const& theme)
        : ThemeRenderer(theme) { }

private:
    virtual void draw_text_button_background(GUI::TextButton const&, Gfx::Painter&) const override;
};

}
