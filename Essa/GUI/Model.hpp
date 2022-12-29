#pragma once

#include <Essa/GUI/Graphics/RichText.hpp>
#include <Essa/LLGL/OpenGL/Texture.hpp>
#include <EssaUtil/UString.hpp>
#include <set>
#include <string>
#include <type_traits>
#include <variant>

namespace GUI {

using Variant = std::variant<Util::UString, Gfx::RichText, llgl::Texture const*>;

struct ModelColumn {
    float width = 15;
    Util::UString name;
};

class Model {
public:
    struct Node {
        // Something to allow user identify what is stored in data
        int type;

        // The pointer to user data
        void const* data;
    };

    virtual ~Model() = default;

    size_t row_count() const;
    size_t root_row_count() const { return children_count(nullptr); }
    GUI::Variant root_data(size_t row, size_t column) { return data(child(nullptr, row), column); }
    size_t depth() const;

    virtual size_t column_count() const = 0;
    virtual ModelColumn column(size_t column) const = 0;
    virtual GUI::Variant data(Node const&, size_t column) const = 0;
    virtual llgl::Texture const* icon(Node const&) const { return nullptr; }
    virtual size_t children_count(Node const*) const = 0;
    virtual Node child(Node const*, size_t idx) const = 0;

private:
    size_t recursive_row_count(Node const*) const;
    size_t recursive_depth(size_t depth, Node const*) const;
};

}
