#pragma once

#include "ScrollableWidget.hpp"

#include <deque>

namespace GUI {

class Console : public ScrollableWidget {
public:
    struct LogLine {
        Util::Color color;
        Util::UString text;
    };

    enum class ScrollToEnd { Yes, No };

    void append_line(LogLine, ScrollToEnd = ScrollToEnd::Yes);
    void append_content(LogLine, ScrollToEnd = ScrollToEnd::Yes);
    void clear();
    void scroll_to_end();

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(
        EML::Object const&, EML::Loader& loader) override;
    virtual void draw(Gfx::Painter&) const override;
    virtual bool accepts_focus() const override { return true; }
    virtual Util::Size2i content_size() const override;

    std::deque<LogLine> m_lines;
};

}
