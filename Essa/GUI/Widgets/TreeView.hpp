#pragma once

#include <Essa/GUI/Overlays/ContextMenu.hpp>
#include <Essa/GUI/Widgets/AbstractListView.hpp>

namespace GUI {

class TreeView : public AbstractListView {
public:
    TreeView();

    virtual void draw(Gfx::Painter&) const override;

    std::function<void(Model::NodeData)> on_click;
    std::function<void(Model::NodeData)> on_double_click;
    std::function<std::optional<ContextMenu>(Model::NodeData)> on_context_menu_request;

    size_t displayed_row_count() const;
    std::pair<std::vector<size_t>, Model::NodeData> displayed_row_at_index(size_t row) const;

    void expand(std::vector<size_t> path) { m_expanded_paths.insert(std::move(path)); }
    bool is_expanded(std::vector<size_t> const& path) const;

    void focus(std::optional<std::vector<size_t>> path) { m_focused_path = std::move(path); }
    std::optional<std::vector<size_t>> focused_row() const { return m_focused_path; }
    std::optional<Model::NodeData> focused_node() const {
        return m_focused_path ? std::optional(model()->node_for_path(*m_focused_path)) : std::nullopt;
    }

private:
    virtual Widget::EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const& event) override;
    virtual Widget::EventHandlerResult on_mouse_double_click(Event::MouseDoubleClick const& event) override;

    virtual Util::Size2i content_size() const override;
    void render_rows(Gfx::Painter& window, float& current_y_pos, std::vector<size_t> path, std::optional<Model::Node> parent) const;

    size_t recursive_displayed_row_count(std::optional<Model::Node>, std::vector<size_t> path) const;
    std::pair<std::vector<size_t>, Model::NodeData>
    recursive_displayed_row_at_index(std::optional<Model::Node>, std::vector<size_t> path, size_t& depth) const;

    std::set<std::vector<size_t>> m_expanded_paths { {} };
    std::optional<std::vector<size_t>> m_focused_path;
    std::optional<size_t> m_last_clicked_row;
};

}
