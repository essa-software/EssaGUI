#include "Model.hpp"

#include <stack>

namespace GUI {

size_t Model::row_count() const { return recursive_row_count({}); }

size_t Model::recursive_row_count(std::optional<Node> node) const {
    size_t children_count = this->children_count(node);
    size_t total_count = children_count;
    for (size_t s = 0; s < children_count; s++) {
        auto child = this->child(node, s);
        total_count += recursive_row_count(Node { s, child });
    }
    return total_count;
}

size_t Model::depth() const { return recursive_depth(0, {}); }

Model::NodeData Model::node_for_path(std::vector<size_t> const& path) const {
    std::optional<Node> current_node;
    assert(!path.empty());
    for (size_t idx : path) {
        auto node_data = child(current_node, idx);
        current_node.emplace(Node { idx, node_data });
    }
    assert(current_node);
    return current_node->data;
}

size_t Model::recursive_depth(size_t depth, std::optional<Node> node) const {
    size_t children_count = this->children_count(node);
    size_t max_depth = 0;
    if (children_count > 0) {
        for (size_t s = 0; s < children_count; s++) {
            auto child = this->child(node, s);
            max_depth = std::max(max_depth, recursive_depth(depth + 1, Node { s, child }));
        }
        return max_depth;
    }
    return depth;
}

}
