#include "events.hpp"
#include "../spec.hpp"
#include "graphics.hpp"

namespace tls
{
#if USE_SDL
inline
#endif
  namespace sdl
{
void
event_handler::handle_events(events& ui_events)
{
    SDL_Event e;
    constexpr float pan_fixed = 20.0f;

    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
            case SDL_EVENT_QUIT:
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if (e.motion.state == SDL_BUTTON_LEFT)
                {
                    ui_events.move.push_back({ e.motion.xrel, e.motion.yrel });
                }
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (e.key.key)
                {
                    case SDLK_Q:
                        ui_events.should_quit = true;
                        break;
                    case SDLK_T:
                        ui_events.toggle_renderer = true;
                        break;
                    case SDLK_LEFT:
                        ui_events.move.push_back({ pan_fixed, pan_fixed });
                        break;
                    case SDLK_RIGHT:
                        ui_events.move.push_back({ -pan_fixed, pan_fixed });
                        break;
                    case SDLK_UP:
                        ui_events.zoom.push_back(
                          { tls::spec::screen_w / 2, tls::spec::screen_h / 2, 0.1f * pan_fixed });
                        break;
                    case SDLK_DOWN:
                        ui_events.zoom.push_back(
                          { tls::spec::screen_w / 2, tls::spec::screen_h / 2, 0.1f * -pan_fixed });
                }
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                if (e.wheel.y != 0)
                    ui_events.zoom.push_back(
                      { e.wheel.mouse_x, e.wheel.mouse_y, e.wheel.y * wheel_y_mult });
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