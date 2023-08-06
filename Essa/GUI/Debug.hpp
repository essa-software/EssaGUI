#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace GUI {

class DebugSwitch {
public:
    DebugSwitch(std::string const& string_id);

    size_t id() const { return m_id; }
    bool get() const;
    void set(bool) const;

    operator bool() const { return get(); }

private:
    size_t m_id;
};

}

#define DBG_ENABLED(name) __debug_##name
#define DBG_DECLARE(name)                      \
    static ::GUI::DebugSwitch __debug_##name { \
#        name                                  \
    }
#define DBG_DECLARE_EXTERN(name) extern ::GUI::DebugSwitch __debug_##name;
#define DBG_DEFINE_GLOBAL(name)                    \
    /*global*/ ::GUI::DebugSwitch __debug_##name { \
#        name                                      \
    }

#define DBG_PRINTLN(name, fmtstr, ...)                              \
    do {                                                            \
        if (DBG_ENABLED(name)) {                                    \
            const auto& vargs = fmt::make_format_args(__VA_ARGS__); \
            fmt::vprint(fmtstr "\n", vargs);                        \
        }                                                           \
    } while (false)
