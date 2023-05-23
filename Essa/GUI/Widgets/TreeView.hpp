#pragma once

#include <Essa/GUI/Widgets/AbstractListView.hpp>

namespace GUI {

class TreeView : public AbstractListView {
public:
    virtual void draw(Gfx::Painter&) const override;

    size_t displayed_row_count() const;
    std::pair<std::vector<size_t>, Model::NodeData> displayed_row_at_index(size_t row) const;
    bool is_expanded(std::vector<size_t> const& path) const;

    void expand(std::vector<size_t> path) { m_expanded_paths.insert(path); }

private:
    virtual Widget::EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const& event) override;

    virtual Util::Size2i content_size() const override;
    void render_rows(Gfx::Painter& window, float& current_y_pos, std::vector<size_t> path, std::optional<Model::Node> parent) const;

    size_t recursive_displayed_row_count(std::optional<Model::Node>, std::vector<size_t> path) const;
    std::pair<std::vector<size_t>, Model::NodeData>
    recursive_displayed_row_at_index(std::optional<Model::Node>, std::vector<size_t> path, size_t& depth) const;

    std::set<std::vector<size_t>> m_expanded_paths { {} };
};

}
