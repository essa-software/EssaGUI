#include "Sphere.hpp"
#include "LLGL/OpenGL/Vertex.hpp"

#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/Renderer/DrawState.hpp>
#include <LLGL/Renderer/Renderer.hpp>
#include <cassert>
#include <vector>

namespace llgl {

Sphere::Sphere()
{
    generate();
}

constexpr auto Stacks = 36;
constexpr auto Sectors = 36;

void Sphere::generate()
{
    float delta_stack_angle = M_PI / Stacks;
    float delta_sector_angle = 2 * M_PI / Sectors;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    for (unsigned stack = 0; stack < Stacks; stack++) {
        for (unsigned sector = 0; sector < Sectors; sector++) {
            auto stack_angle = stack * delta_stack_angle;
            auto sector_angle = sector * delta_sector_angle;
            auto point_position = Util::Vector3f::create_spheric(sector_angle, stack_angle, 1);
            vertices.push_back(Vertex {
                .position = point_position,
                .color = Util::Colors::White,
                .normal = point_position });
            // std::cout << "   --- " << std::sin(sector_angle) << ";" << std::cos(sector_angle) << " ;; " << std::sin(stack_angle) << ";" << std::cos(stack_angle) << std::endl;
            // std::cout << point_position.x << "," << point_position.y << "," << point_position.z << " @ " << stack_angle << "," << sector_angle << std::endl;

            indices.push_back(vertex_index(stack, sector));
            indices.push_back(vertex_index(stack, sector + 1));
            indices.push_back(vertex_index(stack + 1, sector + 1));

            indices.push_back(vertex_index(stack + 1, sector + 1));
            indices.push_back(vertex_index(stack + 1, sector));
            indices.push_back(vertex_index(stack, sector));
        }
    }
    vertices.push_back(Vertex { .position { 0, 0, -1 }, .color = Util::Colors::White, .normal = { 0, 0, -1 } });
    assert(vertices.size() == Sectors * Stacks + 1);

    m_vao.load_vertexes(vertices, indices);

    // std::cout << "----------------------" << std::endl;
    // for (auto& i : m_indices)
    //     std::cout << m_vertices[i].position << std::endl;
}

size_t Sphere::vertex_index(unsigned stack, unsigned sector) const
{
    assert(stack <= Stacks && sector <= Sectors);
    if (stack == Stacks)
        return Stacks * Sectors;
    return stack * Sectors + (sector % Sectors);
}

void Sphere::render(Renderer& renderer, DrawState state) const
{
    renderer.draw_vao(m_vao, opengl::PrimitiveType::Triangles, state);
}

}
