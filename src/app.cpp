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
    while (!events.wheel.empty())
    {
        auto& last = events.wheel.back();
        event.mouse_x = last.mouse_x;
        event.mouse_y = last.mouse_y;
        event.wheel_delta += last.wheel_delta;
        events.wheel.pop_back();
    }
    if (event.wheel_delta != 0)
    {
        ui->scroll_y(event);
    }

    mouse_move mouse_move_delta = { 0, 0 };
    while (!events.mouse.empty())
    {
        mouse_move_delta.x += events.mouse.back().x;
        mouse_move_delta.y += events.mouse.back().y;
        events.mouse.pop_back();
    }
    if (mouse_move_delta.x != mouse_move_delta.y != 0)
        ui->button_left_drag(mouse_move_delta);
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
