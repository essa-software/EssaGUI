#pragma once

#include <Essa/GUI/Graphics/Painter.hpp>

namespace GUI {

class Theme;
class TextButton;
class Widget;

class ThemeRenderer {
public:
    explicit ThemeRenderer(GUI::Theme const& theme)
        : m_theme(theme) { }

    virtual ~ThemeRenderer() = default;

    virtual void draw_text_button_background(GUI::TextButton const&, Gfx::Painter&) const = 0;
    virtual void draw_text_editor_background(GUI::Widget const& widget, Gfx::Painter& painter) const = 0;
    virtual void draw_text_editor_border(GUI::Widget const& widget, bool multiline, Gfx::Painter& painter) const = 0;

protected:
    auto& theme() const {
        return m_theme;
    }

private:
    GUI::Theme const& m_theme;
};

class DefaultThemeRenderer : public ThemeRenderer {
public:
    explicit DefaultThemeRenderer(GUI::Theme const& theme)
        : ThemeRenderer(theme) { }

private:
    virtual void draw_text_button_background(GUI::TextButton const&, Gfx::Painter&) const override;
    virtual void draw_text_editor_background(GUI::Widget const& widget, Gfx::Painter& painter) const override;
    virtual void draw_text_editor_border(GUI::Widget const& widget, bool multiline, Gfx::Painter& painter) const override;
};

}
