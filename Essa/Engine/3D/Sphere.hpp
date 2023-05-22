#pragma once

#include "Model.hpp"
#include <Essa/LLGL/OpenGL/OpenGL.hpp>
#include <Essa/LLGL/OpenGL/Renderer.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <type_traits>
#include <vector>

namespace Essa {

// lat is in range <0; 2π>
// lon is in range <0; π>
template<class Callback = void (*)()>
    requires(std::is_invocable_r<void, Callback, float /*lat_radians*/, float /*lon_radians*/, Essa::Model::Vertex& /*vertex*/>::value
        || std::is_same_v<Callback, void (*)()>)
struct SphereSettings {
    unsigned stacks = 36;
    unsigned sectors = 36;
    // This allows you to modify vertex if you need that
    Callback vertex_callback;
};

class Sphere {
public:
    template<class Callback = void (*)()> explicit Sphere(SphereSettings<Callback> const& s = {}) { generate(s); }

    void render(llgl::Renderer& renderer, llgl::ShaderImpl auto& shader, auto uniforms) const {
        renderer.draw_vertices(m_vao, llgl::DrawState { shader, uniforms, llgl::PrimitiveType::Triangles });
    }

private:
    template<class Callback> void generate(SphereSettings<Callback> const& settings) {
        float delta_stack_angle = M_PI / settings.stacks;
        float delta_sector_angle = 2 * M_PI / settings.sectors;

        std::vector<Model::Vertex> vertices;
        std::vector<unsigned> indices;

        for (unsigned stack = 0; stack < settings.stacks; stack++) {
            for (unsigned sector = 0; sector < settings.sectors; sector++) {
                auto stack_angle = stack * delta_stack_angle - M_PI / 2;
                auto sector_angle = sector * delta_sector_angle - M_PI;
                auto point_position
                    = Util::Point3f::create_spheric(Util::Angle::radians(stack_angle), Util::Angle::radians(sector_angle), 1);
                Model::Vertex vertex { point_position, Util::Colors::White, {}, point_position.to_vector() };
                if constexpr (!std::is_same_v<Callback, void (*)()>) {
                    settings.vertex_callback(stack_angle, sector_angle, vertex);
                }
                vertices.push_back(vertex);
                // std::cout << "   --- " << std::sin(sector_angle) << ";" << std::cos(sector_angle) << " ;; " << std::sin(stack_angle) <<
                // ";" << std::cos(stack_angle) << std::endl; std::cout << point_position.x << "," << point_position.y << "," <<
                // point_position.z << " @ " << stack_angle << "," << sector_angle << std::endl;

                indices.push_back(vertex_index(settings, stack, sector));
                indices.push_back(vertex_index(settings, stack, sector + 1));
                indices.push_back(vertex_index(settings, stack + 1, sector + 1));

                indices.push_back(vertex_index(settings, stack + 1, sector + 1));
                indices.push_back(vertex_index(settings, stack + 1, sector));
                indices.push_back(vertex_index(settings, stack, sector));
            }
        }
        Model::Vertex bottom_pole_vertex { { 0, 0, -1 }, Util::Colors::White, {}, { 0, 0, -1 } };
        if constexpr (!std::is_same_v<Callback, void (*)()>) {
            settings.vertex_callback(M_PI / 2, 0, bottom_pole_vertex);
        }
        vertices.push_back(bottom_pole_vertex);
        assert(vertices.size() == settings.sectors * settings.stacks + 1);

        m_vao.upload_vertices(vertices, indices);

        // std::cout << "----------------------" << std::endl;
        // for (auto& i : m_indices)
        //     std::cout << m_vertices[i].position << std::endl;
    }

    template<class Callback> size_t vertex_index(SphereSettings<Callback> const& settings, unsigned stack, unsigned sector) const {
        assert(stack <= settings.stacks && sector <= settings.sectors);
        if (stack == settings.stacks)
            return settings.stacks * settings.sectors;
        return stack * settings.sectors + (sector % settings.sectors);
    }

    llgl::VertexArray<Model::Vertex> m_vao;
};

}
