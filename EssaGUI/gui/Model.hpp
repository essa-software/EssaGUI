#pragma once

#include <EssaUtil/UString.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <string>
#include <variant>

namespace GUI {

using Variant = std::variant<Util::UString, llgl::opengl::Texture const*>;

class Model {
public:
    virtual ~Model() = default;

    virtual size_t row_count() const = 0;
    virtual size_t column_count() const = 0;
    virtual Variant data(size_t row, size_t column) const = 0;

    struct Column {
        float width = 15;
        Util::UString name;
    };

    virtual Column column(size_t column) const = 0;
};

}
