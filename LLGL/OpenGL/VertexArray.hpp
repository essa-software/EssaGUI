#pragma once

#include "Extensions.hpp"
#include "OpenGL.hpp"
#include "PrimitiveType.hpp"
#include "Vertex.hpp"
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <fmt/ostream.h>
#include <type_traits>
#include <utility>

namespace llgl {

namespace Detail {

struct GLAttrType {
    GLint size;
    GLenum type;
};

template<class T>
struct CppTypeToGL {
};

template<size_t C, class T>
struct CppTypeToGL<Util::Detail::Vector<C, T>> {
    static GLAttrType get() {
        return { .size = C, .type = CppTypeToGL<T>::get().type };
    }
};

template<>
struct CppTypeToGL<Util::Color> {
    static GLAttrType get() {
        return { .size = 4, .type = GL_UNSIGNED_BYTE };
    }
};

template<>
struct CppTypeToGL<Util::Colorf> {
    static GLAttrType get() {
        return { .size = 4, .type = GL_FLOAT };
    }
};

template<>
struct CppTypeToGL<float> {
    static GLAttrType get() {
        return { .size = 1, .type = GL_FLOAT };
    }
};

template<size_t Idx>
struct Index { };

}

template<class Vertex>
class VertexArray {
public:
    VertexArray() {
        opengl::ensure_glew();
        glGenVertexArrays(1, &m_vertex_array);
        glGenBuffers(1, &m_vertex_buffer);
    }

    explicit VertexArray(std::initializer_list<Vertex> vertices)
        : VertexArray() {
        upload_vertices(vertices);
    }

    ~VertexArray() {
        if (m_vertex_array) {
            glDeleteVertexArrays(1, &m_vertex_array);
            glDeleteBuffers(1, &m_vertex_buffer);
        }
        if (m_index_buffer) {
            glDeleteBuffers(1, &m_index_buffer);
        }
    }
    VertexArray(VertexArray const&) = delete;
    VertexArray& operator=(VertexArray const&) = delete;
    VertexArray(VertexArray&& other) {
        m_vertex_array = std::exchange(other.m_vertex_array, 0);
        m_vertex_buffer = std::exchange(other.m_vertex_buffer, 0);
        m_index_buffer = std::exchange(other.m_index_buffer, 0);
        m_vertex_count = std::exchange(other.m_vertex_count, 0);
    }
    VertexArray& operator=(VertexArray&& other) {
        if (this == &other)
            return *this;
        m_vertex_array = std::exchange(other.m_vertex_array, 0);
        m_vertex_buffer = std::exchange(other.m_vertex_buffer, 0);
        m_index_buffer = std::exchange(other.m_index_buffer, 0);
        m_vertex_count = std::exchange(other.m_vertex_count, 0);
        return *this;
    }

    void upload_vertices(std::span<Vertex const> vertices) {
        bind();
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
        bind_attributes();
        m_vertex_count = vertices.size();
    }

    void upload_vertices(std::span<Vertex const> vertices, std::span<unsigned const> indices) {
        bind();
        upload_vertices(vertices);
        if (!m_index_buffer) {
            glGenBuffers(1, &m_index_buffer);
        }
        m_vertex_count = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
    }

    void bind() const {
        glBindVertexArray(m_vertex_array);
    }

    void draw(llgl::PrimitiveType type) const {
        // fmt::print("VAO: Drawing {} vertices with pt={}\n", m_vertex_count, static_cast<int>(type));
        bind();
        if (m_index_buffer)
            glDrawElements(static_cast<GLenum>(type), m_vertex_count, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(static_cast<GLenum>(type), 0, m_vertex_count);
    }

private:
    void bind_attributes() {
        bind();
        bind_attributes(Detail::Index<Vertex::AttributeCount - 1> {});
    }

    template<class T>
    inline void bind_attribute(size_t attrid, size_t offset) {
        glEnableVertexAttribArray(attrid);
        Detail::GLAttrType attrtype = Detail::CppTypeToGL<T>::get();
        // fmt::print("glVertexAttribPointer #{} {}/{}\n", attrid, offset, Vertex::stride());
        glVertexAttribPointer(attrid, attrtype.size, attrtype.type, GL_FALSE, Vertex::stride(), reinterpret_cast<void*>(offset));
    }

    template<size_t Idx>
    inline void bind_attributes(Detail::Index<Idx>) {
        bind_attribute<typename Vertex::template AttributeType<Idx>>(Idx, Vertex::template offset<Idx>());
        bind_attributes(Detail::Index<Idx - 1> {});
    }

    inline void bind_attributes(Detail::Index<0>) {
        bind_attribute<typename Vertex::template AttributeType<0>>(0, Vertex::template offset<0>());
    }

    unsigned m_vertex_array = 0;
    unsigned m_vertex_buffer = 0;
    unsigned m_index_buffer = 0;
    size_t m_vertex_count = 0;
};

}
