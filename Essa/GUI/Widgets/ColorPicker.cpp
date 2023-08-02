#include "ColorPicker.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>

#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Slider.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <Essa/GUI/Widgets/ValueSlider.hpp>
#include <EssaUtil/Color.hpp>

namespace GUI {

class ColorField : public Widget {
public:
    void set_color(Util::Color color) { m_color = color; }

private:
    Util::Color m_color;

    virtual void draw(Gfx::Painter& window) const override;
};

void ColorField::draw(Gfx::Painter& window) const {
    Gfx::RectangleDrawOptions options;
    options.fill_color = m_color;
    window.deprecated_draw_rectangle(local_rect().cast<float>(), options);
}

class ColorPickerDialog : public WindowRoot {
public:
    explicit ColorPickerDialog(WidgetTreeRoot&);

    Util::Color color() const;
    void set_color(Util::Color color);

    static std::optional<Util::Color> exec(HostWindow&, Util::Color initial_color);

private:
    enum class Mode { RGB, HSV };

    void update_controls(Mode);

    bool m_ok_clicked = false;

    ColorField* m_field {};
    ValueSlider* m_r_slider = nullptr;
    ValueSlider* m_g_slider = nullptr;
    ValueSlider* m_b_slider = nullptr;
    ValueSlider* m_h_slider = nullptr;
    ValueSlider* m_s_slider = nullptr;
    ValueSlider* m_v_slider = nullptr;
};

std::optional<Util::Color> ColorPickerDialog::exec(HostWindow& window, Util::Color initial_color) {
    auto dialog = window.open_overlay<ColorPickerDialog>();
    dialog.window_root.set_color(initial_color);
    dialog.overlay.show_modal();
    return dialog.window_root.m_ok_clicked ? dialog.window_root.color() : std::optional<Util::Color> {};
}

ColorPickerDialog::ColorPickerDialog(WidgetTreeRoot& window)
    : WindowRoot(window) {
    (void)load_from_eml_resource(GUI::Application::the().resource_manager().require<EML::EMLResource>("ColorPicker.eml"));

    auto& main_widget = static_cast<Container&>(*this->main_widget());
    auto& main_container = main_widget.find<Container>("main_container");
    auto& ok_button = main_widget.find<TextButton>("ok_button");
    auto& cancel_button = main_widget.find<TextButton>("cancel_button");

    {
        // TODO: Port to EML
        m_field = main_container.add_widget<ColorField>();

        auto* sliders_container = main_container.add_widget<Container>();
        sliders_container->set_layout<VerticalBoxLayout>().set_spacing(10);

        {
            auto create_color_slider = [&](Util::UString component, int max, Mode mode) {
                auto slider = sliders_container->add_widget<ValueSlider>();
                slider->set_min(0);
                slider->set_max(max);
                slider->set_name_textfield_size(20.0_px);
                slider->set_unit_textfield_size(0.0_px);
                slider->set_step(1);
                slider->set_name(component);
                slider->on_change = [this, mode](double) { update_controls(mode); };
                return slider;
            };

            m_r_slider = create_color_slider("R", 255, Mode::RGB);
            m_g_slider = create_color_slider("G", 255, Mode::RGB);
            m_b_slider = create_color_slider("B", 255, Mode::RGB);
            m_h_slider = create_color_slider("H", 360, Mode::HSV);
            m_s_slider = create_color_slider("S", 100, Mode::HSV);
            m_v_slider = create_color_slider("V", 100, Mode::HSV);
        }
    }

    ok_button.on_click = [this]() {
        m_ok_clicked = true;
        close();
    };
    cancel_button.on_click = [this]() { close(); };
}

void ColorPickerDialog::update_controls(Mode mode) {
    if (mode == Mode::HSV) {
        int h = m_h_slider->value();
        float s = m_s_slider->value() / 100;
        float v = m_v_slider->value() / 100;
        auto rgb = Util::HSV { h, s, v }.to_rgb();
        m_r_slider->set_value(rgb.r, NotifyUser::No);
        m_g_slider->set_value(rgb.g, NotifyUser::No);
        m_b_slider->set_value(rgb.b, NotifyUser::No);
    }
    else {
        auto hsv = color().to_hsv();
        m_h_slider->set_value(hsv.hue, NotifyUser::No);
        m_s_slider->set_value(hsv.saturation * 100, NotifyUser::No);
        m_v_slider->set_value(hsv.value * 100, NotifyUser::No);
    }

    m_field->set_color(color());
}

Util::Color ColorPickerDialog::color() const {
    uint8_t r = m_r_slider->value();
    uint8_t g = m_g_slider->value();
    uint8_t b = m_b_slider->value();
    return Util::Color { r, g, b };
}

void ColorPickerDialog::set_color(Util::Color color) {
    m_r_slider->set_value(color.r);
    m_g_slider->set_value(color.g);
    m_b_slider->set_value(color.b);
}

void ColorPicker::on_init() {
    on_click = [this]() {
        auto color = ColorPickerDialog::exec(host_window(), m_color);
        if (color) {
            m_color = *color;
            if (on_change)
                on_change(m_color);
        }
    };
}

void ColorPicker::draw(Gfx::Painter& painter) const {
    auto theme_colors = theme().textbox.value(*this);

    theme().renderer().draw_text_editor_background(*this, painter);
    theme().renderer().draw_text_editor_border(*this, false, painter);

    Gfx::RectangleDrawOptions color_rect;
    color_rect.fill_color = m_color;
    painter.deprecated_draw_rectangle({ 4, 4, static_cast<float>(raw_size().y() - 8), static_cast<float>(raw_size().y() - 8) }, color_rect);

    Gfx::Text html_display { Util::UString { m_color.to_html_string() }, resource_manager().fixed_width_font() };
    html_display.set_fill_color(theme_colors.text);
    html_display.set_font_size(theme().label_font_size);
    html_display.align(
        GUI::Align::CenterLeft,
        { static_cast<float>(raw_size().y()), 4, static_cast<float>(raw_size().x() - raw_size().y()),
          static_cast<float>(raw_size().y() - 8) }
    );
    html_display.draw(painter);
}

EML::EMLErrorOr<void> ColorPicker::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_color = TRY(object.get_property("color", EML::Value(Util::Color())).to_color());

    return {};
}

EML_REGISTER_CLASS(ColorPicker);
}
