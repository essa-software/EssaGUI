#include "ColorPicker.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/AST.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/Slider.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <Essa/GUI/Widgets/ValueSlider.hpp>
#include <EssaUtil/Color.hpp>
#include <utility>

namespace GUI {

static unsigned const COLOR_FIELD_TEX_SIZE = 12;

class ColorField : public Widget {
public:
    void set_color(Util::HSV color) {
        m_color = color;
    }

    // position in range 0..1
    std::function<void(Util::Point2f)> on_change;

private:
    virtual void draw(Gfx::Painter&) const override;
    virtual EventHandlerResult on_mouse_button_press(GUI::Event::MouseButtonPress const&) override;
    virtual EventHandlerResult on_mouse_button_release(GUI::Event::MouseButtonRelease const&) override;
    virtual EventHandlerResult on_mouse_move(GUI::Event::MouseMove const&) override;

    llgl::Texture generate_texture() const;

    // position in range 0..1
    Util::HSV color_from_position(Util::Point2f p) const {
        return Util::HSV {
            .hue = m_color.hue,
            .saturation = static_cast<float>(p.x()),
            .value = 1 - static_cast<float>(p.y()),
        };
    }
    Util::Point2f color_to_position(Util::HSV hsv) const {
        return { hsv.saturation, 1 - hsv.value };
    }

    Util::HSV m_color;
    bool m_dragging = false;
};

llgl::Texture ColorField::generate_texture() const {
    auto image = llgl::Image::create_uninitialized({ COLOR_FIELD_TEX_SIZE, COLOR_FIELD_TEX_SIZE });
    for (unsigned y = 0; y < COLOR_FIELD_TEX_SIZE; y++) {
        for (unsigned x = 0; x < COLOR_FIELD_TEX_SIZE; x++) {
            image.set_pixel({ x, y }, color_from_position(Util::Point2f { x, y } / static_cast<float>(COLOR_FIELD_TEX_SIZE - 1)).to_rgb());
        }
    }
    return llgl::Texture::create_from_image(image);
}

void ColorField::draw(Gfx::Painter& painter) const {
    auto texture = generate_texture();
    texture.set_filtering(llgl::Texture::Filtering::Linear);
    texture.set_wrap(llgl::Texture::Wrap::ClampToEdge);

    using namespace Gfx::Drawing;
    painter.draw(Rectangle(local_rect().cast<float>().inflated(-1), Fill::textured(texture)));
    auto color_pos = color_to_position(m_color);
    color_pos.set_x(color_pos.x() * static_cast<float>(raw_size().x() - 1));
    color_pos.set_y(color_pos.y() * static_cast<float>(raw_size().y() - 1));
    color_pos = color_pos.rounded();

    painter.draw(Rectangle(
        { 0, color_pos.y() - 1, static_cast<float>(raw_size().x()), 3 }, //
        Fill::solid(theme().placeholder.with_alpha(200))
    ));
    painter.draw(Rectangle(
        { color_pos.x() - 1, 0, 3, static_cast<float>(raw_size().y()) }, //
        Fill::solid(theme().placeholder.with_alpha(200))
    ));

    painter.draw(Rectangle({ 0, color_pos.y(), static_cast<float>(raw_size().x()), 1 }, Fill::solid(Util::Colors::Black)));
    painter.draw(Rectangle({ color_pos.x(), 0, 1, static_cast<float>(raw_size().y()) }, Fill::solid(Util::Colors::Black)));

    // flush because of texture
    painter.render();
    painter.reset();

    theme().renderer().draw_text_editor_border(*this, true, painter);
}

Widget::EventHandlerResult ColorField::on_mouse_button_press(GUI::Event::MouseButtonPress const&) {
    m_dragging = true;
    return Widget::EventHandlerResult::Accepted;
}
Widget::EventHandlerResult ColorField::on_mouse_button_release(GUI::Event::MouseButtonRelease const&) {
    m_dragging = false;
    return Widget::EventHandlerResult::Accepted;
}

Widget::EventHandlerResult ColorField::on_mouse_move(GUI::Event::MouseMove const& event) {
    if (!m_dragging) {
        return Widget::EventHandlerResult::NotAccepted;
    }
    auto pos = (event.local_position() - Util::Vector2i(1, 1)).cast<float>();
    pos.set_x(std::clamp<float>(pos.x() / static_cast<float>(raw_size().x() - 1), 0, 1));
    pos.set_y(std::clamp<float>(1 - pos.y() / static_cast<float>(raw_size().y() - 1), 0, 1));
    on_change(pos);
    return Widget::EventHandlerResult::NotAccepted;
}

class ColorSlider : public Widget {
public:
    void set_color(Util::HSV color) {
        m_color = color;
    }

    std::function<void(float fac)> on_change;

    // fac is in range <0..1>
    using GetColorFunction = std::function<Util::Color(float fac)>;

    explicit ColorSlider(GetColorFunction f, Util::Orientation o)
        : m_get_color(f)
        , m_orientation(o) { }

private:
    virtual void draw(Gfx::Painter&) const override;
    virtual EventHandlerResult on_mouse_button_press(GUI::Event::MouseButtonPress const&) override;
    virtual EventHandlerResult on_mouse_button_release(GUI::Event::MouseButtonRelease const&) override;
    virtual EventHandlerResult on_mouse_move(GUI::Event::MouseMove const&) override;

    llgl::Texture generate_texture() const;

    GetColorFunction m_get_color;
    Util::Orientation m_orientation;
    Util::HSV m_color;
    bool m_dragging = false;
};

llgl::Texture ColorSlider::generate_texture() const {
    auto image = llgl::Image::create_uninitialized({ COLOR_FIELD_TEX_SIZE, 1 });
    for (unsigned x = 0; x < COLOR_FIELD_TEX_SIZE; x++) {
        image.set_pixel({ x, 0 }, m_get_color(static_cast<float>(x) / (COLOR_FIELD_TEX_SIZE - 1)));
    }
    return llgl::Texture::create_from_image(image);
}

void ColorSlider::draw(Gfx::Painter& painter) const {
    auto texture = generate_texture();
    texture.set_filtering(llgl::Texture::Filtering::Linear);
    texture.set_wrap(llgl::Texture::Wrap::ClampToEdge);

    std::vector<Gfx::Vertex> vertices;
    vertices.resize(6);
    vertices[0] = Gfx::Vertex({ 1, 1 }, Util::Colors::White, {});
    vertices[1] = Gfx::Vertex({ raw_size().x() - 1, raw_size().y() - 1 }, Util::Colors::White, {});
    vertices[2] = Gfx::Vertex({ 1, raw_size().y() - 1 }, Util::Colors::White, {});
    vertices[3] = Gfx::Vertex({ 1, 1 }, Util::Colors::White, {});
    vertices[4] = Gfx::Vertex({ raw_size().x() - 1, 1 }, Util::Colors::White, {});
    vertices[5] = Gfx::Vertex({ raw_size().x() - 1, raw_size().y() - 1 }, Util::Colors::White, {});

    if (m_orientation == Util::Orientation::Vertical) {
        vertices[0].tex_coord() = { 0, 0 };
        vertices[1].tex_coord() = { 1, 1 };
        vertices[2].tex_coord() = { 1, 0 };
        vertices[3].tex_coord() = { 0, 0 };
        vertices[4].tex_coord() = { 0, 1 };
        vertices[5].tex_coord() = { 1, 1 };
    }
    else {
        vertices[0].tex_coord() = { 0, 0 };
        vertices[1].tex_coord() = { 1, 1 };
        vertices[2].tex_coord() = { 0, 1 };
        vertices[3].tex_coord() = { 0, 0 };
        vertices[4].tex_coord() = { 1, 0 };
        vertices[5].tex_coord() = { 1, 1 };
    }

    painter.draw_vertices(llgl::PrimitiveType::Triangles, vertices, &texture);

    using namespace Gfx::Drawing;
    float fac = m_color.hue / 360.f * (raw_size().main(m_orientation) - 1);
    if (m_orientation == Util::Orientation::Vertical) {
        painter.draw(Rectangle(
            { 0, fac - 1, static_cast<float>(raw_size().x()), 3 }, //
            Fill::solid(theme().placeholder.with_alpha(200))
        ));
        painter.draw(Rectangle({ 0, fac, static_cast<float>(raw_size().x()), 1 }, Fill::solid(Util::Colors::Black)));
    }
    else {
        painter.draw(Rectangle(
            { fac - 1, 0, 3, static_cast<float>(raw_size().y()) }, //
            Fill::solid(theme().placeholder.with_alpha(200))
        ));
        painter.draw(Rectangle({ fac, 0, 1, static_cast<float>(raw_size().y()) }, Fill::solid(Util::Colors::Black)));
    }

    painter.render();
    painter.reset();

    theme().renderer().draw_text_editor_border(*this, true, painter);
}

Widget::EventHandlerResult ColorSlider::on_mouse_button_press(GUI::Event::MouseButtonPress const&) {
    m_dragging = true;
    return Widget::EventHandlerResult::Accepted;
}
Widget::EventHandlerResult ColorSlider::on_mouse_button_release(GUI::Event::MouseButtonRelease const&) {
    m_dragging = false;
    return Widget::EventHandlerResult::Accepted;
}

Widget::EventHandlerResult ColorSlider::on_mouse_move(GUI::Event::MouseMove const& event) {
    if (!m_dragging) {
        return Widget::EventHandlerResult::NotAccepted;
    }
    float fac = static_cast<float>(event.local_position().main(m_orientation)) / static_cast<float>(raw_size().main(m_orientation) - 1);
    on_change(std::clamp<float>(fac, 0, 1));

    return Widget::EventHandlerResult::NotAccepted;
}

class ColorPickerDialog : public WindowRoot {
public:
    explicit ColorPickerDialog(WidgetTreeRoot&, Util::Color initial_color);

    Util::Color color() const;
    void set_color(Util::Color color);

    std::function<void(Util::Color const&)> on_change;

private:
    enum class Mode { RGB, HSV };

    void update_controls(Mode);

    Util::Color m_initial_color;
    ColorField* m_field {};
    ColorSlider* m_hue_slider {};
    Widget* m_current_color_widget {};
    ValueSlider* m_r_slider = nullptr;
    ValueSlider* m_g_slider = nullptr;
    ValueSlider* m_b_slider = nullptr;
    ValueSlider* m_h_slider = nullptr;
    ValueSlider* m_s_slider = nullptr;
    ValueSlider* m_v_slider = nullptr;
    Textbox* m_html_textbox = nullptr;
};

ColorPickerDialog::ColorPickerDialog(WidgetTreeRoot& window, Util::Color initial_color)
    : WindowRoot(window)
    , m_initial_color(initial_color) {
    (void)load_from_eml_resource(GUI::Application::the().resource_manager().require<EML::EMLResource>("ColorPicker.eml"));

    auto& main_widget = static_cast<Container&>(*this->main_widget());
    auto& ok_button = main_widget.find<TextButton>("ok_button");
    auto& cancel_button = main_widget.find<TextButton>("cancel_button");

    {
        auto& main_container = main_widget.find<Container>("main_container");
        main_container.set_layout<HorizontalBoxLayout>().set_spacing(5);
        // TODO: Port to EML
        {
            auto& left_container = *main_container.add_widget<Container>();
            left_container.set_layout<VerticalBoxLayout>().set_spacing(10);
            {
                auto& field_and_hue_container = *left_container.add_widget<Container>();
                field_and_hue_container.set_layout<HorizontalBoxLayout>().set_spacing(10);
                m_field = field_and_hue_container.add_widget<ColorField>();
                m_field->on_change = [this](Util::Point2f p) {
                    m_s_slider->set_value(p.x() * 100);
                    m_v_slider->set_value(p.y() * 100);
                };
                m_hue_slider = field_and_hue_container.add_widget<ColorSlider>(
                    [](float fac) { return Util::HSV(static_cast<int>(fac * 360), 1, 1).to_rgb(); }, Util::Orientation::Vertical
                );
                m_hue_slider->set_size({ 32_px, Util::Length::Auto });
                m_hue_slider->on_change = [this](float fac) { m_h_slider->set_value(fac * 360); };
            }
            {
                auto& current_old_color_container = *left_container.add_widget<Container>();
                current_old_color_container.set_size({ Util::Length::Auto, 40_px });
                current_old_color_container.set_layout<VerticalBoxLayout>();
                {
                    {
                        auto& current_color_container = *current_old_color_container.add_widget<Container>();
                        current_color_container.set_layout<HorizontalBoxLayout>();
                        auto& tf1 = *current_color_container.add_widget<Textfield>();
                        tf1.set_size({ 50_perc, Util::Length::Auto });
                        tf1.set_content("Current:");
                        m_current_color_widget = current_color_container.add_widget<Widget>();
                        m_current_color_widget->set_background_color(m_initial_color);
                    }
                    {
                        auto& old_color_container = *current_old_color_container.add_widget<Container>();
                        old_color_container.set_layout<HorizontalBoxLayout>();
                        auto& tf2 = *old_color_container.add_widget<Textfield>();
                        tf2.set_size({ 50_perc, Util::Length::Auto });
                        tf2.set_content("Old:");
                        old_color_container.add_widget<Widget>()->set_background_color(m_initial_color);
                    }
                }
            }
        }
        {
            auto& right_container = *main_container.add_widget<Container>();
            right_container.set_layout<VerticalBoxLayout>().set_spacing(10);
            {
                auto* sliders_container = right_container.add_widget<Container>();
                sliders_container->set_layout<VerticalBoxLayout>().set_spacing(10);
                sliders_container->set_size({ Util::Length::Initial, Util::Length::Auto });
                auto create_color_slider = [&](Util::UString component, int max, Mode mode) {
                    auto* slider = sliders_container->add_widget<ValueSlider>();
                    slider->set_min(0);
                    slider->set_max(max);
                    slider->set_name_textfield_size(20.0_px);
                    slider->set_unit_textfield_size(0.0_px);
                    slider->set_step(1);
                    slider->set_name(std::move(component));
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
            auto& html_value = *right_container.add_widget<GUI::Container>();
            auto& layout = html_value.set_layout<GUI::HorizontalBoxLayout>();
            layout.set_spacing(10);
            layout.set_padding({ 0, 5, 0, 0 });
            html_value.set_size({ Util::Length::Auto, 40_px });
            {
                auto& html_label = *html_value.add_widget<GUI::Textfield>();
                html_label.set_content("HTML:");
                m_html_textbox = html_value.add_widget<GUI::Textbox>();
                m_html_textbox->set_type(GUI::Textbox::Type::TEXT);
                m_html_textbox->set_content(m_initial_color.to_html_string());
                m_html_textbox->on_change = [this](Util::UString const& text) {
                    if (!text.starts_with("#") || text.size() != 7) {
                        return;
                    }
                    auto hex = text.substring(1).parse<int>(16);
                    if (hex.is_error()) {
                        return;
                    }
                    auto color = Util::Color(hex.release_value() << 8);
                    set_color(color);
                };
            }
        }
    }

    ok_button.on_click = [this]() { close(); };
    cancel_button.on_click = [this]() {
        set_color(m_initial_color);
        close();
    };

    set_color(initial_color);
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

    Util::HSV hsv {
        static_cast<int>(m_h_slider->value()),
        static_cast<float>(m_s_slider->value() / 100.0),
        static_cast<float>(m_v_slider->value() / 100.0),
    };

    m_field->set_color(hsv);
    m_hue_slider->set_color(hsv);
    m_current_color_widget->set_background_color(hsv.to_rgb());
    m_html_textbox->set_content(color().to_html_string(), GUI::NotifyUser::No);
    if (on_change) {
        on_change(color());
    }
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

ColorPicker::~ColorPicker() {
    if (m_color_picker_window) {
        m_color_picker_window->close();
    }
}

void ColorPicker::on_init() {
    on_click = [this]() {
        auto window = GUI::Application::the().open_host_window<ColorPickerDialog>(m_color);
        window.window.set_always_on_top();
        window.root.on_change = [this](Util::Color const& color) {
            m_color = color;
            if (on_change)
                on_change(color);
        };
        window.window.on_close = [this]() { m_color_picker_window = nullptr; };
        m_color_picker_window = &window.window;
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
