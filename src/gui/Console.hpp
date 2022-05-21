#pragma once

#include "Widget.hpp"
#include <deque>

namespace GUI {

class Console : public Widget {
public:
    explicit Console(Container& c)
        : Widget(c) { }

    struct LogLine {
        sf::Color color;
        std::string text;
    };

    void append_line(LogLine);
    void clear();

private:
    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow&) const override;

    std::deque<LogLine> m_lines;
    float m_scroll {};

    virtual bool accepts_focus() const override { return true; }

    float content_size() const;
    float scroll_area_size() const;
};

}
