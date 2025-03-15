#pragma once

#include "../types.hpp"
#include <deque>
#include <list>

namespace tls
{

#if USE_SDL
inline
#endif
  namespace sdl
{
struct event_handler : event_handler_ifc
{
    void handle_events(events& events) override;

    const float wheel_y_mult = 6.5f;
};

struct timer : timer_ifc
{
    [[nodiscard]] size_t get_ms_since_start() const override;
    void wait_ms(size_t ms) const override;
};
}
} // namespace tls
