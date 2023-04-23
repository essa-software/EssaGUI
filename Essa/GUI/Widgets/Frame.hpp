#pragma once

#include <Essa/GUI/Widgets/Container.hpp>

namespace GUI {

class Frame : public Container {
public:
    static constexpr float BorderRadius = 20;

private:
    virtual void draw(Gfx::Painter&) const override;
    virtual Boxi intrinsic_padding() const override { return Boxi::all_equal(BorderRadius); }
    virtual bool steals_focus() const override { return true; }

protected:
    using Container::add_created_widget;
    using Container::add_widget;
    using Container::clear_layout;
    using Container::get_layout;
    using Container::shrink;
    using Container::widgets;
};

}
