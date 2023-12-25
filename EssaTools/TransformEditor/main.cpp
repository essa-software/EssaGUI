#include <Essa/Engine/3D/Model.hpp>
#include <Essa/Engine/3D/Shaders/Basic.hpp>
#include <Essa/Engine/3D/Shaders/Lighting.hpp>
#include <Essa/Engine/3D/Shapes.hpp>
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/Graphics/Drawing/Rectangle.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Overlays/FileExplorer.hpp>
#include <Essa/GUI/Overlays/FilePrompt.hpp>
#include <Essa/GUI/TextEditing/TextPosition.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/RadioGroup.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <Essa/GUI/Widgets/WorldView.hpp>
#include <Essa/LLGL/OpenGL/DrawState.hpp>
#include <Essa/LLGL/OpenGL/PrimitiveType.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Transform.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <EssaUtil/Error.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Stream/MemoryStream.hpp>
#include <EssaUtil/Stream/Reader.hpp>

template<> class fmt::formatter<Util::Matrix4x4f> : public fmt::formatter<std::string_view> {
public:
    template<typename FormatContext> constexpr auto format(Util::Matrix4x4f const& p, FormatContext& ctx) const {
        fmt::format_to(
            ctx.out(), "| {:5.2f} {:5.2f} {:5.2f} {:5.2f} |\n", p.element(0, 0), p.element(0, 1), p.element(0, 2), p.element(0, 3)
        );
        fmt::format_to(
            ctx.out(), "| {:5.2f} {:5.2f} {:5.2f} {:5.2f} |\n", p.element(1, 0), p.element(1, 1), p.element(1, 2), p.element(1, 3)
        );
        fmt::format_to(
            ctx.out(), "| {:5.2f} {:5.2f} {:5.2f} {:5.2f} |\n", p.element(2, 0), p.element(2, 1), p.element(2, 2), p.element(0, 3)
        );
        fmt::format_to(
            ctx.out(), "| {:5.2f} {:5.2f} {:5.2f} {:5.2f} |\n", p.element(3, 0), p.element(3, 1), p.element(3, 2), p.element(3, 3)
        );
        return ctx.out();
    }
};

static Util::OsErrorOr<Util::Matrix4x4f> parse_line(Util::UString const& l) {
    if (l.is_empty() || l.starts_with("#")) {
        return Util::Matrix4x4f::identity();
    }
    auto line = l.encode_buffer();
    Util::ReadableMemoryStream stream { line };
    Util::TextReader reader { stream };

    auto consume_float = [&]() -> Util::OsErrorOr<float> {
        TRY(reader.consume_while(isspace));
        return TRY(reader.consume_while([](uint32_t cp) { return isdigit(cp) || cp == '.' || cp == '-' || cp == 'e'; })).parse<float>();
    };

    auto command = MUST(reader.consume_while(isalpha));

    if (command == "mat") {
        float data[4 * 4];
        for (auto& d : data) {
            d = TRY(consume_float());
        }
        return Util::Matrix4x4f(data);
    }
    if (command == "persp") {
        float fov = TRY(consume_float());
        float aspect = TRY(consume_float());
        float near = TRY(consume_float());
        float far = TRY(consume_float());
        return llgl::Projection::perspective(llgl::PerspectiveArgs { fov, aspect, near, far }, {}).matrix();
    }
    if (command == "ortho") {
        float left = TRY(consume_float());
        float right = TRY(consume_float());
        float top = TRY(consume_float());
        float bottom = TRY(consume_float());
        float near = TRY(consume_float());
        float far = TRY(consume_float());
        return llgl::Projection::ortho(llgl::OrthoArgs { { left, top, right - left, bottom - top }, near, far }, {}).matrix();
    }
    if (command == "scale") {
        float fac = TRY(consume_float());
        return llgl::Transform {}.scale(fac).matrix();
    }
    if (command == "scalex") {
        float fac = TRY(consume_float());
        return llgl::Transform {}.scale_x(fac).matrix();
    }
    if (command == "scaley") {
        float fac = TRY(consume_float());
        return llgl::Transform {}.scale_y(fac).matrix();
    }
    if (command == "scalez") {
        float fac = TRY(consume_float());
        return llgl::Transform {}.scale_z(fac).matrix();
    }
    if (command == "translate") {
        float x = TRY(consume_float());
        float y = TRY(consume_float());
        float z = TRY(consume_float());
        return llgl::Transform {}.translate({ x, y, z }).matrix();
    }
    if (command == "rotx") {
        float deg = TRY(consume_float());
        return llgl::Transform {}.rotate_x(static_cast<float>(deg / 180 * M_PI)).matrix();
    }
    if (command == "roty") {
        float deg = TRY(consume_float());
        return llgl::Transform {}.rotate_y(static_cast<float>(deg / 180 * M_PI)).matrix();
    }
    if (command == "rotz") {
        float deg = TRY(consume_float());
        return llgl::Transform {}.rotate_z(static_cast<float>(deg / 180 * M_PI)).matrix();
    }

    return Util::OsError { 0, "invalid command" };
}

static Util::OsErrorOr<Util::Matrix4x4f> parse_transform(Util::UString const& string) {
    Util::Matrix4x4f matrix;

    std::optional<Util::OsError> error;
    string.for_each_line([&](std::span<uint32_t> l) {
        auto result = parse_line(Util::UString { l });
        if (result.is_error()) {
            error = result.release_error();
            return;
        }
        matrix = matrix * result.release_value();
    });
    if (error) {
        return *error;
    }
    return matrix;
}

class CameraView : public GUI::Widget {
public:
    enum class CoordinateSystem { Local, World, View, Clip };

    CameraView();

    void set_model(Util::Matrix4x4f model) { m_uniforms.set_model(model); }
    void set_view(Util::Matrix4x4f view) { m_uniforms.set_view(view); }
    void set_projection(Util::Matrix4x4f projection) { m_uniforms.set_projection(projection); }

    void set_object(Essa::Model const* object) { m_object = object; }
    void set_axises_coordinate_system(CoordinateSystem system) { m_axises_coord_system = system; }

private:
    virtual void draw(Gfx::Painter& painter) const override;

    Essa::Model const* m_object = &resource_manager().require<Essa::Model>("cube.obj");
    Essa::Shaders::Lighting::Uniforms m_uniforms;
    llgl::VertexArray<Essa::Model::Vertex> m_axises_vao;
    CoordinateSystem m_axises_coord_system = CoordinateSystem::Clip;
};

CameraView::CameraView() {
    constexpr auto AxisSize = 100;
    std::vector<Essa::Model::Vertex> vertices {
        { { -AxisSize, 0, 0 }, Util::Colors::Red, {}, {} },   { { AxisSize, 0, 0 }, Util::Colors::Red, {}, {} },
        { { 0, -AxisSize, 0 }, Util::Colors::Green, {}, {} }, { { 0, AxisSize, 0 }, Util::Colors::Green, {}, {} },
        { { 0, 0, -AxisSize }, Util::Colors::Blue, {}, {} },  { { 0, 0, AxisSize }, Util::Colors::Blue, {}, {} },
    };
    Essa::Shapes::add_wireframe_cube(vertices);
    m_axises_vao.upload_vertices(vertices);
}

void CameraView::draw(Gfx::Painter& painter) const {
    using namespace Gfx::Drawing;
    painter.draw(Rectangle(local_rect().cast<float>(), Fill::solid(Util::Colors::Black)));

    painter.render();
    painter.reset();
    {
        OpenGL::Enable(GL_DEPTH_TEST);
        OpenGL::Clear(GL_DEPTH_BUFFER_BIT);
        static auto& shader = Essa::Shaders::Lighting::load(resource_manager());
        auto axises_uniforms = m_uniforms;
        if (m_axises_coord_system > CoordinateSystem::Local) {
            axises_uniforms.set_model({});
        }
        if (m_axises_coord_system > CoordinateSystem::World) {
            axises_uniforms.set_view({});
        }
        if (m_axises_coord_system > CoordinateSystem::View) {
            axises_uniforms.set_projection({});
        }
        painter.renderer().draw_vertices(m_axises_vao, llgl::DrawState { shader, axises_uniforms, llgl::PrimitiveType::Lines });
        m_object->render(painter.renderer(), shader, m_uniforms);
        OpenGL::Disable(GL_DEPTH_TEST);
    }

    {
        Gfx::Text text(Util::UString(fmt::format("model:\n{}", m_uniforms.model())), GUI::Application::the().fixed_width_font());
        text.set_font_size(15);
        text.set_position({ 0, 20 });
        text.draw(painter);
    }

    {
        Gfx::Text text(Util::UString(fmt::format("view:\n{}", m_uniforms.view())), GUI::Application::the().fixed_width_font());
        text.set_font_size(15);
        text.set_position({ 250, 20 });
        text.draw(painter);
    }

    {
        Gfx::Text text(Util::UString(fmt::format("projection:\n{}", m_uniforms.projection())), GUI::Application::the().fixed_width_font());
        text.set_font_size(15);
        text.set_position({ 500, 20 });
        text.draw(painter);
    }

    {
        Gfx::Text text(Util::UString(fmt::format("aspect: {}", raw_size().aspect_ratio())), GUI::Application::the().fixed_width_font());
        text.set_font_size(15);
        text.align(GUI::Align::BottomRight, Util::Rectf::centered(raw_size().to_vector().to_point().cast<float>(), {}));
        text.draw(painter);
    }

    theme().renderer().draw_text_editor_border(*this, false, painter);
}

EML_REGISTER_CLASS(CameraView)

class MainWidget : public GUI::Container {
    virtual void on_init() {
        assert(load_from_eml_resource(resource_manager().require<EML::EMLResource>("MainWidget.eml")));
        m_model_editor = find_widget_of_type_by_id_recursively<GUI::TextEditor>("model_editor");
        m_model_editor->on_change = [&](Util::UString const&) { update_view(); };
        m_view_editor = find_widget_of_type_by_id_recursively<GUI::TextEditor>("view_editor");
        m_view_editor->on_change = [&](Util::UString const&) { update_view(); };
        m_projection_editor = find_widget_of_type_by_id_recursively<GUI::TextEditor>("projection_editor");
        m_projection_editor->on_change = [&](Util::UString const&) { update_view(); };
        m_camera_view = find_widget_of_type_by_id_recursively<CameraView>("camera_view");

        auto* load_model = find_widget_of_type_by_id_recursively<GUI::TextButton>("load_model");
        load_model->on_click = [this]() {
            auto path = GUI::FileExplorer::get_path_to_open(&host_window());
            if (!path) {
                return;
            }
            host_window().set_active();
            m_camera_view->set_object(&resource_manager().require_external<Essa::Model>(path->string()));
        };

        auto* axis_coordinate_system = find_widget_of_type_by_id_recursively<GUI::RadioGroup>("axis_coordinate_system");
        axis_coordinate_system->on_change
            = [this](size_t idx) { m_camera_view->set_axises_coordinate_system(static_cast<CameraView::CoordinateSystem>(idx)); };
    }

    void update_view() {
        auto model = parse_transform(m_model_editor->content());
        if (model.is_error()) {
            m_model_editor->set_error_spans({
                GUI::TextEditor::ErrorSpan {
                    .type = GUI::TextEditor::ErrorSpan::Type::Error,
                    .range = GUI::TextRange { { 0, 0 }, { 0, 1 } },
                },
            });
            fmt::print("{}\n", model.error());
        }
        else {
            m_camera_view->set_model(model.release_value());
        }

        auto view = parse_transform(m_view_editor->content());
        if (view.is_error()) {
            m_view_editor->set_error_spans({
                GUI::TextEditor::ErrorSpan {
                    .type = GUI::TextEditor::ErrorSpan::Type::Error,
                    .range = GUI::TextRange { { 0, 0 }, { 0, 1 } },
                },
            });
            fmt::print("{}\n", view.error());
        }
        else {
            m_camera_view->set_view(view.release_value());
        }

        auto projection = parse_transform(m_projection_editor->content());
        if (projection.is_error()) {
            m_projection_editor->set_error_spans({
                GUI::TextEditor::ErrorSpan {
                    .type = GUI::TextEditor::ErrorSpan::Type::Error,
                    .range = GUI::TextRange { { 0, 0 }, { 0, 1 } },
                },
            });
            fmt::print("{}\n", projection.error());
        }
        else {
            m_camera_view->set_projection(projection.release_value());
        }
    }

    GUI::TextEditor* m_model_editor = nullptr;
    GUI::TextEditor* m_view_editor = nullptr;
    GUI::TextEditor* m_projection_editor = nullptr;
    CameraView* m_camera_view = nullptr;
};

int main() {
    GUI::SimpleApplication<MainWidget> app { "Transform Editor" };
    app.main_widget().set_background_color(app.theme().window_background);
    app.run();

    return 0;
}
