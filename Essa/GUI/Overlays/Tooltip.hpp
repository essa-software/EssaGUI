#pragma once

#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/WindowRoot.hpp>
#include <string>

namespace GUI {

class Widget;
class Textfield;

struct Tooltip {
    Util::UString text;

    Tooltip(Util::UString t)
        : text(std::move(t)) { }
};

class TooltipOverlay : public WindowRoot {
public:
    explicit TooltipOverlay(WidgetTreeRoot&, Tooltip);

    void set_text(Util::UString);
    Util::UString text() const;

private:
    Textfield* m_textfield = nullptr;
    Util::UString m_text;
};

}
