#include "events.hpp"
#include "graphics.hpp"

namespace tls
{
#if USE_SDL
inline
#endif
  namespace sdl
{
void
event_handler::handle_events(events& events)
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
            case SDL_EVENT_QUIT:
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if (e.motion.state == SDL_BUTTON_LEFT)
                {
                    events.mouse.push_back({ e.motion.xrel, e.motion.yrel });
                }
                /*
                else if (e.motion.state == 4)
                {
                    events.wheel.push_back(-e.motion.yrel);
                }
                */
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (e.key.key)
                {
                    case SDLK_Q:
                        events.should_quit = true;
                        break;
                    case SDLK_T:
                        events.toggle_renderer = true;
                        break;
                }
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                if (e.wheel.y != 0)
                    // events.wheel.push_back(e.wheel.y * wheel_y_mult);
                    events.wheel.push_back(e.wheel.y * 2.0f);

                break;
            default:;
        }
    }
}

size_t
timer::get_ms_since_start() const
{
    return SDL_GetTicks();
}

void
timer::wait_ms(size_t ms) const
{
    SDL_Delay(ms);
}
}
} // namespace tls