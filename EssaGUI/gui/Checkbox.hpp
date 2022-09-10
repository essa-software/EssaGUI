#pragma once
#include "Button.hpp"
#include <EssaUtil/Units.hpp>

namespace GUI {

class Checkbox : public Button {
public:
    virtual void on_init() override { set_toggleable(true); }

    virtual void draw(GUI::Window& window) const override;

    Util::UString get_caption() const { return m_caption; }
    void set_caption(Util::UString const& str) { m_caption = str; }

    virtual Theme::ButtonColors default_button_colors() const override;

    enum class Style {
        CROSS,
        MARK
    };

    Style get_style() const { return m_style; }
    void set_style(const Style& style) { m_style = style; }
    Length get_box_size() const { return m_box_size; }
    void box_size(Length box_size) { m_box_size = box_size; }

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    Util::UString m_caption = "";

    Style m_style = Style::CROSS;
    Length m_box_size = 12.0_px;
};

}
