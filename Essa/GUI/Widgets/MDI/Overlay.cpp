#include "Overlay.hpp"

#include <Essa/GUI/Widgets/MDI/Host.hpp>

namespace GUI::MDI {

HostWindow const& GUI::MDI::Overlay::host_window() const { return host().host_window(); }

}
