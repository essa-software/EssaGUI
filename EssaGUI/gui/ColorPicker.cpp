#include "ColorPicker.hpp"

#include "Container.hpp"
#include "Textfield.hpp"
#include "ToolWindow.hpp"
#include <EssaGUI/gfx/Window.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/NotifyUser.hpp>
#include <EssaGUI/gui/Slider.hpp>
#include <EssaGUI/gui/TextButton.hpp>
#include <EssaGUI/gui/ValueSlider.hpp>
#include <EssaUtil/Color.hpp>

namespace GUI {

class ColorField : public Widget {
public:
    explicit ColorField(Container& c)
        : Widget(c) { }

    void set_color(Util::Color color) { m_color = color; }

private:
    Util::Color m_color;

    virtual void draw(GUI::Window& window) const override;
};

void ColorField::draw(GUI::Window& window) const {
    RectangleDrawOptions options;
    options.fill_color = m_color;
    window.draw_rectangle(local_rect(), options);
}

class ColorPickerDialog : public ToolWindow {
public:
    explicit ColorPickerDialog(GUI::Window& wnd);

    Util::Color color() const;
    void set_color(Util::Color color);

    static std::optional<Util::Color> exec(Util::Color initial_color);

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

std::optional<Util::Color> ColorPickerDialog::exec(Util::Color initial_color) {
    auto& dialog = Application::the().open_overlay<ColorPickerDialog>();
    dialog.set_color(initial_color);
    dialog.run();
    return dialog.m_ok_clicked ? dialog.color() : std::optional<Util::Color> {};
}

ColorPickerDialog::ColorPickerDialog(GUI::Window& wnd)
    : ToolWindow(wnd, "GUI::ColorPickerDialog") {
    set_title("Pick a color");
    set_size({ 500, 305 });
    center_on_screen();

    auto& container = set_main_widget<Container>();
    auto& container_layout = container.set_layout<VerticalBoxLayout>();
    container_layout.set_spacing(10);
    container_layout.set_padding(10);

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
                    auto slider = sliders_container->add_widget<ValueSlider>(0, max, 1,
                        ValueSliderOptions {
                            .name_textfield_size = 20.0_px,
                            .unit_textfield_size = 0.0_px });
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

        submit_container->set_size({ Length::Auto, 30.0_px });

        auto ok_button = submit_container->add_widget<TextButton>();
        ok_button->set_size({ 80.0_px, Length::Auto });
        ok_button->set_content("OK");
        ok_button->on_click = [this]() {
            m_ok_clicked = true;
            close();
        };

        auto cancel_button = submit_container->add_widget<TextButton>();
        cancel_button->set_size({ 80.0_px, Length::Auto });
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

ColorPicker::ColorPicker(Container& c)
    : Button(c) {
    on_click = [this]() {
        auto color = ColorPickerDialog::exec(m_color);
        if (color) {
            m_color = *color;
            if (on_change)
                on_change(m_color);
        }
    };
}

Util::Color ColorPicker::color() const {
    return m_color;
}

void ColorPicker::set_color(Util::Color color) {
    m_color = color;
}

void ColorPicker::draw(GUI::Window& window) const {
    auto theme_colors = theme().textbox.value(*this);

    RectangleDrawOptions background_rect;
    background_rect.fill_color = theme_colors.background;
    background_rect.outline_color = theme_colors.foreground;
    background_rect.outline_thickness = -1;
    window.draw_rectangle(local_rect(), background_rect);

    RectangleDrawOptions color_rect;
    color_rect.fill_color = m_color;
    window.draw_rectangle({ 4, 4, size().y() - 8, size().y() - 8 }, color_rect);

    TextDrawOptions html_display;
    html_display.fill_color = theme_colors.text;
    html_display.text_align = Align::CenterLeft;
    html_display.font_size = theme().label_font_size;
    window.draw_text_aligned_in_rect(Util::UString { m_color.to_html_string() }, { size().y(), 4, size().x() - size().y(), size().y() - 8 }, resource_manager().fixed_width_font(), html_display);
}
}
