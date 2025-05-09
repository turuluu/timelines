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
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                mouse_drag = true;
            }
            break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                mouse_drag = false;
            }
            break;
            case SDL_EVENT_MOUSE_MOTION:
            {
                if (mouse_drag)
                    ui_events.drag.push_back({ e.motion.xrel, e.motion.yrel });
                else
                    ui_events.move.push_back({e.motion.x, e.motion.y});
                    // ui_events.move.push_back({ e.motion.xrel, e.motion.yrel });
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
                    case SDLK_A:
                    case SDLK_LEFT:
                        ui_events.drag.push_back({ pan_fixed, pan_fixed });
                        break;
                    case SDLK_D:
                    case SDLK_RIGHT:
                        ui_events.drag.push_back({ -pan_fixed, pan_fixed });
                        break;
                    case SDLK_W:
                    case SDLK_UP:
                        ui_events.zoom.push_back(
                          { tls::spec::screen_w / 2, tls::spec::screen_h / 2, 0.1f * pan_fixed });
                        break;
                    case SDLK_S:
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