#include "ColorPicker.hpp"

#include <EssaGUI/Application.hpp>
#include <EssaGUI/Graphics/Text.hpp>
#include <EssaGUI/Graphics/Window.hpp>
#include <EssaGUI/NotifyUser.hpp>
#include <EssaGUI/Overlays/ToolWindow.hpp>
#include <EssaGUI/Widgets/Container.hpp>
#include <EssaGUI/Widgets/Slider.hpp>
#include <EssaGUI/Widgets/TextButton.hpp>
#include <EssaGUI/Widgets/Textfield.hpp>
#include <EssaGUI/Widgets/ValueSlider.hpp>
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
    window.draw_rectangle(local_rect(), options);
}

class ColorPickerDialog : public ToolWindow {
public:
    explicit ColorPickerDialog(HostWindow&);

    Util::Color color() const;
    void set_color(Util::Color color);

    static std::optional<Util::Color> exec(HostWindow&, Util::Color initial_color);

private:
    enum class Mode {
        RGB,
        HSV
    };

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
    auto& dialog = window.open_overlay<ColorPickerDialog>();
    dialog.set_color(initial_color);
    dialog.run();
    return dialog.m_ok_clicked ? dialog.color() : std::optional<Util::Color> {};
}

ColorPickerDialog::ColorPickerDialog(HostWindow& window)
    : ToolWindow(window, "GUI::ColorPickerDialog") {
    set_title("Pick a color");
    set_size({ 500, 305 });
    center_on_screen();

    auto& container = set_main_widget<Container>();
    auto& container_layout = container.set_layout<VerticalBoxLayout>();
    container_layout.set_spacing(10);
    container_layout.set_padding(Boxf::all_equal(10));

    {
        auto main_container = container.add_widget<Container>();
        auto& main_container_layout = main_container->set_layout<HorizontalBoxLayout>();
        main_container_layout.set_spacing(10);
        {
            m_field = main_container->add_widget<ColorField>();

            auto* sliders_container = main_container->add_widget<Container>();
            sliders_container->set_layout<VerticalBoxLayout>().set_spacing(10);

            {
                auto create_color_slider = [&](Util::UString component, int max, Mode mode) {
                    auto slider = sliders_container->add_widget<ValueSlider>();
                    slider->set_min(0);
                    slider->set_max(max);
                    slider->set_name_textfield_size(20.0_px);
                    slider->set_unit_textfield_size(0.0_px);
                    slider->set_name(component);
                    slider->on_change = [this, mode](double) {
                        update_controls(mode);
                    };
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

        auto submit_container = container.add_widget<Container>();
        auto& submit_container_layout = submit_container->set_layout<HorizontalBoxLayout>();
        submit_container_layout.set_spacing(10);
        submit_container_layout.set_content_alignment(BoxLayout::ContentAlignment::BoxEnd);

        submit_container->set_size({ Util::Length::Auto, 30.0_px });

        auto ok_button = submit_container->add_widget<TextButton>();
        ok_button->set_size({ 80.0_px, Util::Length::Auto });
        ok_button->set_content("OK");
        ok_button->on_click = [this]() {
            m_ok_clicked = true;
            close();
        };

        auto cancel_button = submit_container->add_widget<TextButton>();
        cancel_button->set_size({ 80.0_px, Util::Length::Auto });
        cancel_button->set_content("Cancel");
        cancel_button->on_click = [this]() {
            close();
        };
    }
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

void ColorPicker::draw(Gfx::Painter& window) const {
    auto theme_colors = theme().textbox.value(*this);

    Gfx::RectangleDrawOptions background_rect;
    background_rect.fill_color = theme_colors.background;
    background_rect.outline_color = theme_colors.foreground;
    background_rect.outline_thickness = -1;
    window.draw_rectangle(local_rect(), background_rect);

    Gfx::RectangleDrawOptions color_rect;
    color_rect.fill_color = m_color;
    window.draw_rectangle({ 4, 4, raw_size().y() - 8, raw_size().y() - 8 }, color_rect);

    Gfx::Text html_display { Util::UString { m_color.to_html_string() }, resource_manager().fixed_width_font() };
    html_display.set_fill_color(theme_colors.text);
    html_display.set_font_size(theme().label_font_size);
    html_display.align(GUI::Align::CenterLeft, { raw_size().y(), 4, raw_size().x() - raw_size().y(), raw_size().y() - 8 });
    html_display.draw(window);
}

EML::EMLErrorOr<void> ColorPicker::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_color = TRY(object.get_property("color", Util::Color()).to_color());

    return {};
}

EML_REGISTER_CLASS(ColorPicker);

}