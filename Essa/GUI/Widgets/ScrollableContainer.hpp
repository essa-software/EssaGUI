#pragma once

#include <Essa/GUI/Widgets/ScrollableWidget.hpp>

namespace GUI {

// A container will scroll its child widgets if it's overflowing.
//
// The underlying widget DOESN'T get update() calls. Use timers for
// this. In general, I'm going to deprecate update() in favor of timers
// or Application::on_tick.
class ScrollableContainer : public ScrollableWidget {
public:
    ScrollableContainer();

    template<class T>
        requires(std::is_base_of_v<Widget, T>)
    T& set_widget() {
        m_widget = std::make_shared<T>();
        m_widget->set_parent(*this);
        m_widget->init();
        set_needs_relayout();
        return *static_cast<T*>(m_widget.get());
    }

    std::shared_ptr<GUI::Widget> widget() { return m_widget; }

private:
    virtual Util::Size2i content_size() const override;
    virtual Widget::EventHandlerResult do_handle_event(Event const& event) override;
    virtual void draw(Gfx::Painter&) const override;
    virtual void do_relayout() override;
    virtual void dump(std::ostream& out, unsigned depth) override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    std::shared_ptr<GUI::Widget> m_widget;
};

}
