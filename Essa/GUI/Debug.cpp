#include "Debug.hpp"
#include <EssaUtil/UString.hpp>
#include <map>
#include <set>
#include <type_traits>

namespace GUI {

class DebugSettings {
public:
    void set(DebugSwitch const& ds, bool value) {
        if (ds.id() >= m_switches.size()) {
            m_switches.resize(ds.id() + 1);
        }
        m_switches[ds.id()] = value;
    }

    bool get(DebugSwitch const& ds) {
        if (ds.id() >= m_switches.size()) {
            return false;
        }
        return m_switches[ds.id()];
    }

private:
    std::vector<bool> m_switches;
};

static DebugSettings s_debug_settings;
static std::map<std::string, DebugSwitch const*> s_debug_switches;
static size_t s_current_id = 0;

bool is_enabled_from_env(std::string const& switch_) {
    static std::set<std::string> essa_debug_switches = []() {
        auto essa_debug = getenv("ESSA_DEBUG");
        if (!essa_debug || strcmp(essa_debug, "") == 0) {
            return std::set<std::string> {};
        }
        Util::UString env { essa_debug };
        std::set<std::string> set;
        env.for_each_split(",", [&](std::span<uint32_t const> span) { set.insert(Util::UString { span }.encode()); });
        return set;
    }();
    return essa_debug_switches.contains(switch_);
}

DBG_DECLARE(DumpSwitches);

DebugSwitch::DebugSwitch(std::string const& string_id)
    : m_id(++s_current_id) {
    s_debug_switches.insert({ string_id, this });
    if (is_enabled_from_env(string_id)) {
        set(true);
    }
    if (DBG_ENABLED(DumpSwitches)) {
        fmt::print("{} = {}\n", string_id, get());
    }
}

bool DebugSwitch::get() const { return s_debug_settings.get(*this); }

void DebugSwitch::set(bool value) const { s_debug_settings.set(*this, value); }

}
