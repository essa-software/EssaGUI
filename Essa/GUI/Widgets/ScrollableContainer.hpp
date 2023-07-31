#pragma once

#include <Essa/GUI/Widgets/ScrollableWidget.hpp>

namespace GUI {

class ScrollableContainer : public ScrollableWidget {
public:
    ScrollableContainer();

    template<class T> T& set_widget() {
        m_widget = std::make_shared<T>();
        m_widget->set_window_root(window_root());
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
    virtual void dump(unsigned depth) override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    std::shared_ptr<GUI::Widget> m_widget;
};

}
