#pragma once

#include <LLGL/Core/Updateable.h>
#include <list>
#include <memory>

namespace llgl
{

class EventLoop : public Updateable
{
public:

    template<class EventType, class Callback>
    void add_event_handler(Callback&& callback)
    {
    }

    template<class EventType> requires EventType::Result
    typename EventType::Result fire_event(EventType const& event)
    {

    }

    template<class EventType, class... Args> requires requires(Args&&... args) { EventType{std::forward<Args>(args)...}; }
    typename EventType::Result fire_new_event(Args&&... args)
    {
        return fire_event(EventType{std::forward<Args>(args)...});
    }

    virtual void update() override;
};

}
