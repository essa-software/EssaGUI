#pragma once

#include "ScrollableWidget.hpp"

#include <deque>

namespace GUI {

class Console : public ScrollableWidget {
public:
    explicit Console(Container& c)
        : ScrollableWidget(c) { }

    struct LogLine {
        sf::Color color;
        Util::UString text;
    };

    void append_line(LogLine);
    void append_content(LogLine);
    void clear();

private:
    virtual void draw(GUI::SFMLWindow&) const override;
    virtual bool accepts_focus() const override { return true; }
    virtual float content_height() const override;

    std::deque<LogLine> m_lines;
};

}
