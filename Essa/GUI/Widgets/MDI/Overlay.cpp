#include "Overlay.hpp"

#include <Essa/GUI/Widgets/MDI/Host.hpp>
#include <Essa/GUI/WindowRoot.hpp>

namespace GUI::MDI {

Overlay::Overlay(Host& host, std::string id)
    : m_host(host)
    , m_id(std::move(id)) {

    // Dirty hack until Dialog refactoring is finished.
    // (Derived constructors that create widgets may need these)
    if (host.maybe_window_root()) {
        set_root(std::make_unique<WindowRoot>(*this));
    }
}

void Overlay::setup(Util::UString /*title*/, Util::Size2u size) { m_size = size.cast<int>(); }

void Overlay::center_on_screen() {
    auto& window = host();
    set_position((window.raw_size() / 2 - size() / 2.f).to_vector().to_point());
}

HostWindow const& Overlay::host_window() const { return host().host_window(); }

}
