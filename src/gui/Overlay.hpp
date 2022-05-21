#pragma once

#include "WidgetTreeRoot.hpp"

namespace GUI {

class Overlay : public WidgetTreeRoot {
public:
    Overlay(sf::RenderWindow& wnd, std::string id)
        : WidgetTreeRoot(wnd)
        , m_id(std::move(id)) { }

    void close() {
        m_closed = true;
        quit();
    }
    bool is_closed() const { return m_closed; }

    std::string id() const { return m_id; }

    std::function<void()> on_close;

private:
    std::string m_id;
    bool m_closed = false;
};

}
