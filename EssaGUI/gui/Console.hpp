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

    void append_line(LogLine);
    void append_content(LogLine);
    void clear();

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
    virtual void draw(GUI::Window&) const override;
    virtual bool accepts_focus() const override { return true; }
    virtual float content_height() const override;

    std::deque<LogLine> m_lines;
};

}
