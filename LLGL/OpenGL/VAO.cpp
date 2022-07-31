#include "VAO.hpp"

#include "Extensions.hpp"

#include <EssaUtil/DelayedInit.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/Renderer/Renderer.hpp>

#include <cassert>
#include <iostream>
#include <vector>

namespace llgl::opengl {

VAO::VAO()
{
    ensure_glew();
}

VAO::VAO(std::span<Vertex const> vertexes, Usage usage)
{
    ensure_glew();
    load_vertexes(vertexes, usage);
}

VAO::VAO(std::span<Vertex const> vertexes, std::span<unsigned const> indices, Usage usage)
{
    ensure_glew();
    load_vertexes(vertexes, indices, usage);
}

VAO::VAO(VAO&& other)
{
    *this = std::move(other);
}

VAO& VAO::operator=(VAO&& other)
{
    if (this == &other)
        return *this;
    m_vertex_array_id = std::exchange(other.m_vertex_array_id, 0);
    m_index_buffer_id = std::exchange(other.m_index_buffer_id, 0);
    m_vertex_buffer_id = std::exchange(other.m_vertex_buffer_id, 0);
    m_vertex_count = std::exchange(other.m_vertex_count, 0);
    return *this;
}

VAO::~VAO()
{
    if (m_vertex_buffer_id) {
        glDeleteBuffers(1, &m_vertex_buffer_id);
    }
    if (m_index_buffer_id) {
        glDeleteBuffers(1, &m_index_buffer_id);
    }
    if (m_vertex_array_id) {
        glDeleteVertexArrays(1, &m_vertex_array_id);
    }
}

void VAO::load_vertexes(std::span<Vertex const> vertexes, Usage usage)
{
    if (!m_vertex_array_id) {
        glGenVertexArrays(1, &m_vertex_array_id);
    }

    bind();
    if (!m_vertex_buffer_id) {
        glGenBuffers(1, &m_vertex_buffer_id);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    //std::cerr << "VAO: Loading " << vertexes.size() << " vertexes" << std::endl;

    auto usage_gl = [&]() {
        switch (usage) {
            case Usage::DynamicDraw:
                return GL_DYNAMIC_DRAW;
            case Usage::StaticDraw:
                return GL_STATIC_DRAW;
        }
        return 0;
    }();

    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(Vertex), vertexes.data(), usage_gl);

    m_vertex_count = vertexes.size();
}

void VAO::load_vertexes(std::span<Vertex const> vertexes, std::span<unsigned const> indices, Usage usage)
{
    load_vertexes(vertexes, usage);

    if (!m_index_buffer_id) {
        glGenBuffers(1, &m_index_buffer_id);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id);
    std::cerr << "VAO: Loading " << indices.size() << " indices" << std::endl;

    auto usage_gl = [&]() {
        switch (usage) {
            case Usage::DynamicDraw:
                return GL_DYNAMIC_DRAW;
            case Usage::StaticDraw:
                return GL_STATIC_DRAW;
        }
        return 0;
    }();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), usage_gl);
    m_vertex_count = indices.size();
}

void VAO::bind() const
{
    assert(m_vertex_array_id);
    glBindVertexArray(m_vertex_array_id);
}

void VAO::bind_with_attributes(AttributeMapping attribute_mapping) const
{
    bind();
    if (!m_attribute_mapping || attribute_mapping != *m_attribute_mapping) {
        m_attribute_mapping = attribute_mapping;

        // FIXME: Make this more flexible
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
        glEnableVertexAttribArray(attribute_mapping.position);
        glEnableVertexAttribArray(attribute_mapping.color);
        glEnableVertexAttribArray(attribute_mapping.tex_coord);
        glEnableVertexAttribArray(attribute_mapping.normal);
        glVertexAttribPointer(attribute_mapping.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));   // x,y
        glVertexAttribPointer(attribute_mapping.color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));         // x,y
        glVertexAttribPointer(attribute_mapping.tex_coord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord)); // x,y
        glVertexAttribPointer(attribute_mapping.normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));       // x,y
    }
}

void VAO::draw(AttributeMapping attribute_mapping, PrimitiveType primitive_type) const
{
    bind_with_attributes(attribute_mapping);
    if (m_index_buffer_id)
        glDrawElements(static_cast<GLenum>(primitive_type), m_vertex_count, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(static_cast<GLenum>(primitive_type), 0, m_vertex_count);
}

}
