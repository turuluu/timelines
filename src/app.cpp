#include "app.hpp"
#include "sdl/graphics.hpp"

namespace tls
{
application::application(struct core& core)
  : core(core)
{
}
application::~application() { utlz::dbg("Application DTOR\n"); }

void
application::process_events()
{
    is_running = !events.should_quit;

    if (events.toggle_renderer)
    {
        ui->toggle_renderer();
        events.toggle_renderer = false;
    }

    wheel_move event{0,0,0};
    while (!events.zoom.empty())
    {
        auto& last = events.zoom.back();
        event.mouse_x = last.mouse_x;
        event.mouse_y = last.mouse_y;
        event.wheel_delta += last.wheel_delta;
        events.zoom.pop_back();
    }
    if (event.wheel_delta != 0)
    {
        ui->zoom(event);
    }

    mouse_move mouse_move_delta = { 0, 0 };
    while (!events.move.empty())
    {
        mouse_move_delta.x += events.move.back().x;
        mouse_move_delta.y += events.move.back().y;
        events.move.pop_back();
    }
    if (mouse_move_delta.x != 0 || mouse_move_delta.y != 0)
        ui->move_viewport(mouse_move_delta);
}

void
application::loop()
{
    ui->last_frame_ms = ui->timer->get_ms_since_start();
    while (is_running)
    {
        ui->wait_until_next_frame();
        event_handler->handle_events(events);
        process_events();
        ui->render();
        ui->last_frame_ms = ui->timer->get_ms_since_start();
    }
}
} // namespace tls
