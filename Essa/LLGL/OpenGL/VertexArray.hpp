#pragma once

#include "Extensions.hpp"
#include "OpenGL.hpp"
#include "PrimitiveType.hpp"
#include "Vertex.hpp"
#include <Essa/LLGL/AbstractOpenGLHelper.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/CoordinateSystem.hpp>
#include <EssaUtil/Vector.hpp>
#include <cassert>
#include <fmt/ostream.h>
#include <type_traits>
#include <utility>

namespace llgl {

namespace Detail {

struct GLAttrType {
    GLint size;
    GLenum type;
};

template<class T> struct CppTypeToGL { };

template<size_t C, class T>
/*deprecated*/ struct CppTypeToGL<Util::Detail::DeprecatedVector<C, T>> {
    static GLAttrType get() { return { .size = C, .type = CppTypeToGL<T>::get().type }; }
};

template<size_t C, class T> struct CppTypeToGL<Util::Detail::Point<C, T>> {
    static GLAttrType get() { return { .size = C, .type = CppTypeToGL<T>::get().type }; }
};

template<size_t C, class T> struct CppTypeToGL<Util::Detail::Vector<C, T>> {
    static GLAttrType get() { return { .size = C, .type = CppTypeToGL<T>::get().type }; }
};

template<size_t C, class T> struct CppTypeToGL<Util::Detail::Size<C, T>> {
    static GLAttrType get() { return { .size = C, .type = CppTypeToGL<T>::get().type }; }
};

template<> struct CppTypeToGL<Util::Color> {
    static GLAttrType get() { return { .size = 4, .type = GL_UNSIGNED_BYTE }; }
};

template<> struct CppTypeToGL<Util::Colorf> {
    static GLAttrType get() { return { .size = 4, .type = GL_FLOAT }; }
};

template<> struct CppTypeToGL<float> {
    static GLAttrType get() { return { .size = 1, .type = GL_FLOAT }; }
};

template<size_t Idx> struct Index { };

}

template<class Vertex> class VertexArray {
public:
    VertexArray() {
        opengl::ensure_glew();
        OpenGL::GenVertexArrays(1, &m_vertex_array);
        OpenGL::GenBuffers(1, &m_vertex_buffer);
    }

    explicit VertexArray(std::initializer_list<Vertex> vertices)
        : VertexArray() {
        upload_vertices(vertices);
    }

    ~VertexArray() {
        if (m_vertex_array) {
            OpenGL::DeleteVertexArrays(1, &m_vertex_array);
            OpenGL::DeleteBuffers(1, &m_vertex_buffer);
        }
        if (m_index_buffer) {
            OpenGL::DeleteBuffers(1, &m_index_buffer);
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
        OpenGL::BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        OpenGL::BufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
        bind_attributes();
        m_vertex_count = vertices.size();
    }

    void upload_vertices(std::span<Vertex const> vertices, std::span<unsigned const> indices) {
        bind();
        upload_vertices(vertices);
        if (!m_index_buffer) {
            OpenGL::GenBuffers(1, &m_index_buffer);
        }
        m_vertex_count = indices.size();
        OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        OpenGL::BufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), (void*)indices.data(), GL_STATIC_DRAW);
    }

    void bind() const { OpenGL::BindVertexArray(m_vertex_array); }

    void draw(llgl::PrimitiveType type) const {
        // fmt::print("VAO: Drawing {} vertices with pt={}\n", m_vertex_count, static_cast<int>(type));
        bind();
        if (m_index_buffer)
            OpenGL::DrawElements(static_cast<GLenum>(type), m_vertex_count, GL_UNSIGNED_INT, nullptr);
        else
            OpenGL::DrawArrays(static_cast<GLenum>(type), 0, m_vertex_count);
    }

    void draw(llgl::PrimitiveType type, size_t first, size_t size) const {
        // fmt::print("VAO: Drawing {} vertices with pt={}\n", m_vertex_count, static_cast<int>(type));
        bind();
        assert(!m_index_buffer);
        OpenGL::DrawArrays(static_cast<GLenum>(type), first, size);
    }

private:
    void bind_attributes() {
        bind();
        bind_attributes(Detail::Index<Vertex::AttributeCount - 1> {});
    }

    template<class T> inline void bind_attribute(size_t attrid, size_t offset) {
        OpenGL::EnableVertexAttribArray(attrid);
        Detail::GLAttrType attrtype = Detail::CppTypeToGL<T>::get();
        // fmt::print("glVertexAttribPointer #{} {}/{}\n", attrid, offset, Vertex::stride());
        OpenGL::VertexAttribPointer(attrid, attrtype.size, attrtype.type, GL_FALSE, Vertex::stride(), reinterpret_cast<void*>(offset));
    }

    template<size_t Idx> inline void bind_attributes(Detail::Index<Idx>) {
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
