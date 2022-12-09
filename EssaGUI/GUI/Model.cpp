#include "Model.hpp"

#include <EssaGUI/GUI/ImageButton.hpp>
#include <stack>

namespace GUI {

size_t Model::row_count() const {
    return recursive_row_count(nullptr);
}

size_t Model::recursive_row_count(Node const* node) const {
    size_t children_count = this->children_count(node);
    size_t total_count = children_count;
    for (size_t s = 0; s < children_count; s++) {
        auto child = this->child(node, s);
        total_count += recursive_row_count(&child);
    }
    return total_count;
}

size_t Model::depth() const {
    return recursive_depth(0, nullptr);
}

size_t Model::recursive_depth(size_t depth, Node const* node) const {
    size_t children_count = this->children_count(node);
    size_t max_depth = 0;
    if (children_count > 0) {
        for (size_t s = 0; s < children_count; s++) {
            auto child = this->child(node, s);
            max_depth = std::max(max_depth, recursive_depth(depth + 1, &child));
        }
        return max_depth;
    }
    return depth;
}

}
