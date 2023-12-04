#pragma once

#include <Essa/GUI/WindowRoot.hpp>

namespace GUI {

class DevTools : public GUI::WindowRoot {
public:
    explicit DevTools(WidgetTreeRoot& wnd);
};

}
