#include <EssaUtil/Color.hpp>
#include <LLGL/OpenGL/Blend.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/OpenGL/Shaders/Basic330Core.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <LLGL/OpenGL/Utils.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/Renderer/BatchRenderer.hpp>
#include <LLGL/Renderer/Renderer.hpp>
#include <LLGL/Window/Window.hpp>

int main() {
    llgl::Window window { { 500, 500 }, "core renderer", { 3, 2 } };

    std::vector<Util::Colorf> color_array;
    color_array.resize(256 * 256);
    for (size_t y = 0; y < 256; y++) {
        for (size_t x = 0; x < 256; x++) {
            color_array[y * 256 + x] = Util::Colorf {
                static_cast<float>((double)rand() / RAND_MAX),
                static_cast<float>((double)rand() / RAND_MAX),
                static_cast<float>((double)rand() / RAND_MAX),
                0.5f
            };
        }
    }
    llgl::opengl::Texture texture = llgl::opengl::Texture::create_from_color_array({ 256, 256 }, std::span<Util::Colorf const> { color_array.data(), color_array.size() }, llgl::opengl::Texture::Format::RGBA);

    llgl::opengl::enable(llgl::opengl::Feature::Blend);
    llgl::opengl::enable(llgl::opengl::Feature::DepthTest);
    llgl::opengl::set_blend_func(llgl::opengl::BlendFunc::SrcAlpha, llgl::opengl::BlendFunc::OneMinusSrcAlpha);
    llgl::opengl::set_clear_color(Util::Color { 255, 128, 128 });
    llgl::BatchRenderer batch_renderer;

    llgl::opengl::shaders::Basic330Core shader;
    batch_renderer.draw_vao(llgl::opengl::VAO { { { llgl::Vertex { { -1.5, -1.5, -15 }, Util::Colors::White, { 0, 0 } },
                                llgl::Vertex { { 1.5, 1.5, -15 }, Util::Colors::White, { 1, 1 } },
                                llgl::Vertex { { 1.5, -1.5, -15 }, Util::Colors::White, { 1, 0 } },
                                llgl::Vertex { { -1.5, -1.5, -10 }, Util::Colors::White, { 0, 0 } },
                                llgl::Vertex { { 1.5, 1.5, -10 }, Util::Colors::White, { 1, 1 } },
                                llgl::Vertex { { 1.5, -1.5, -10 }, Util::Colors::White, { 1, 0 } } } } },
        llgl::opengl::PrimitiveType::Triangles, { .shader = &shader, .texture = &texture });

    for (;;) {
        llgl::Event event;
        while (window.poll_event(event)) {
        }
        llgl::opengl::clear(llgl::opengl::ClearMask::Color | llgl::opengl::ClearMask::Depth);

        window.renderer().apply_projection(llgl::Projection::perspective({ 1.22, window.aspect(), 0.1, 20 }, window.rect()));
        window.renderer().render_object(batch_renderer, {});
        window.display();
    }
    return 0;
}
