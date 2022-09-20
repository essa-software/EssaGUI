#pragma once

#include <EssaGUI/gui/AbstractListView.hpp>

namespace GUI {

class TreeView : public AbstractListView {
public:
    virtual void draw(GUI::Window&) const override;

private:
    void render_rows(GUI::Window& window, float& current_y_pos, size_t depth, Model::Node const* parent) const;
};

}
