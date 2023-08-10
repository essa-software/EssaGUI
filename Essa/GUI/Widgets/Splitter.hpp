#pragma once

#include <Essa/GUI/Widgets/Container.hpp>
#include <EssaUtil/Orientation.hpp>

namespace GUI {

class Splitter : public Container {
public:
    explicit Splitter(Util::Orientation orientation)
        : m_orientation(orientation) { }

private:
    // Don't unnecesarily expose implementation details (i.e that Splitter derives from Container)
    using Container::clear_layout;
    using Container::get_layout;
    using Container::set_layout;

    Util::Rectu splitter_rect(size_t index) const;

    virtual EventHandlerResult on_mouse_move(Event::MouseMove const&) override;
    virtual EventHandlerResult on_mouse_button_press(Event::MouseButtonPress const&) override;
    virtual EventHandlerResult on_mouse_button_release(Event::MouseButtonRelease const&) override;
    virtual void draw(Gfx::Painter&) const override;
    virtual void relayout() override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    Util::Orientation m_orientation;
    std::optional<size_t> m_splitter_dragged;
    int m_drag_start = 0;
};

class HorizontalSplitter : public Splitter {
public:
    HorizontalSplitter()
        : Splitter(Util::Orientation::Horizontal) { }
};

class VerticalSplitter : public Splitter {
public:
    VerticalSplitter()
        : Splitter(Util::Orientation::Vertical) { }
};

}
