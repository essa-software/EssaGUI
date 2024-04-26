#pragma once

#include <cstddef>
#include <fmt/format.h>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace GUI {

class DebugSwitch {
public:
    DebugSwitch(std::string const& string_id);

    size_t id() const {
        return m_id;
    }
    bool get() const;
    void set(bool) const;

    operator bool() const {
        return get();
    }

    static std::map<std::string, DebugSwitch const*>& switches();

private:
    size_t m_id;
};

}

#define DBG_ENABLED(name) __debug_##name
#define DBG_DECLARE(name)                      \
    static ::GUI::DebugSwitch __debug_##name { \
        #name                                  \
    }
#define DBG_DECLARE_EXTERN(name) extern ::GUI::DebugSwitch __debug_##name;
#define DBG_DEFINE_GLOBAL(name)                    \
    /*global*/ ::GUI::DebugSwitch __debug_##name { \
        #name                                      \
    }
#define DBG_PRINTLN(name, fmtstr, ...)                                    \
    {                                                                     \
        if (DBG_ENABLED(name)) {                                          \
            fmt::vprint(fmtstr "\n", fmt::make_format_args(__VA_ARGS__)); \
        }                                                                 \
    }
