#pragma once

#include <EssaUtil/UString.hpp>
#include <vector>

namespace GUI {

class DevToolsObject {
public:
    virtual ~DevToolsObject() = default;
    virtual std::vector<DevToolsObject const*> dev_tools_children() const = 0;
    virtual Util::UString dev_tools_name() const = 0;
    size_t dev_tools_children_count() const { return dev_tools_children().size(); }
};

}
