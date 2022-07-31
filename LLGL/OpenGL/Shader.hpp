#pragma once

#include <GL/glew.h>

#include <EssaUtil/Color.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Vector.hpp>
#include <span>
#include <string>

namespace llgl::opengl {

class ShaderObject {
public:
    enum Type {
        Vertex = GL_VERTEX_SHADER,
        TessControl = GL_TESS_CONTROL_SHADER,
        TessEvaluation = GL_TESS_EVALUATION_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER
    };

    ShaderObject(std::string const& source, Type type);
    ShaderObject(ShaderObject const&) = delete;
    ShaderObject& operator=(ShaderObject const&) = delete;
    ShaderObject(ShaderObject&& other) = default;
    ShaderObject& operator=(ShaderObject&&) = default;
    ~ShaderObject();

    Type type() const { return m_type; }
    bool valid() const { return m_valid; }
    unsigned id() const { return m_id; }

private:
    Type m_type;
    bool m_valid = false;
    unsigned m_id = 0;
};

struct AttributeMapping {
    unsigned position;
    unsigned color;
    unsigned tex_coord;
    unsigned normal;

    bool operator==(AttributeMapping const&) const = default;
};

class Program {
public:
    ~Program();
    explicit Program(std::span<ShaderObject const> shader_objects);
    Program(Program const&) = delete;
    Program& operator=(Program const&) = delete;
    Program(Program&& Program) = default;
    Program& operator=(Program&&) = default;

    void bind() const;
    static Program const* current();

    bool valid() const { return m_valid; }
    unsigned id() const { return m_id; }

private:
    bool m_valid = false;
    unsigned m_id = 0;
};

class ShaderScope;

// OpenGL program + data required to properly set up for rendering (attribute
// mapping and uniform data).
class Shader {
public:
    Shader(Program& program)
        : m_program(program)
    {
    }

    virtual ~Shader() = default;

    virtual AttributeMapping attribute_mapping() const = 0;

    Program const& program() const { return m_program; }
    void bind(ShaderScope&) const;

private:
    virtual void on_bind(ShaderScope&) const = 0;

    Program& m_program;
};

class ShaderScope {
public:
    ShaderScope(Shader const& shader)
        : m_shader(shader)
    {
        shader.bind(*this);
    }
    ~ShaderScope()
    { /*Not applicable since OpenGL 3.2*/
    }

    int uniform_location(std::string const&);
    void set_uniform(std::string const&, Util::Matrix4x4f);
    enum CurrentTextureTag {
        CurrentTexture
    };
    void set_uniform(std::string const&, CurrentTextureTag);
    void set_uniform(std::string const&, bool);
    void set_uniform(std::string const&, int);
    void set_uniform(std::string const&, float);
    void set_uniform(std::string const&, Util::Vector2f);
    void set_uniform(std::string const&, Util::Vector3f);
    void set_uniform(std::string const&, Util::Colorf);

private:
    Shader const& m_shader;
};

}
