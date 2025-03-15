#pragma once
#include <deque>

#include "geometry.hpp"

namespace tls
{
struct colour
{
    u8 border = 255;
    u8 fill = 255;
};

struct mouse_move
{
    float x;
    float y;
};

struct events
{
    bool should_quit{ false };
    bool toggle_renderer{ false };
    std::deque<mouse_move> mouse;
    std::deque<float> wheel;
};

struct event_handler_ifc
{
    virtual ~event_handler_ifc() { printf("Event Handler DTOR\n"); }
    virtual void handle_events(events& events) = 0;
};

struct timer_ifc
{
    virtual ~timer_ifc() = default;
    virtual void wait_ms(size_t delay_ms) const = 0;

    /**
     * time delta since initiation
     */
    [[nodiscard]] virtual size_t get_ms_since_start() const = 0;
};
}