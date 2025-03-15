#include "app.hpp"
#include "sdl/graphics.hpp"

namespace tls
{
application::application(struct core& core)
  : core(core)
{
}
application::~application() { printf("Application DTOR\n"); }

void
application::process_events()
{
    is_running = !events.should_quit;

    if (events.toggle_renderer)
    {
        ui->toggle_renderer();
        events.toggle_renderer = false;
    }

    i32 y_scroll_delta = 0;
    while (!events.wheel.empty())
    {
        y_scroll_delta += events.wheel.back();
        events.wheel.pop_back();
    }
    if (y_scroll_delta != 0)
    {
        float x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        ui->scroll_y(y_scroll_delta, x, y);
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
    static int counter = 0;
    ui->last_frame_ms = ui->timer->get_ms_since_start();
    while (is_running)
    {
        // std::cout << counter++ << std::endl;
        ui->wait_until_next_frame();
        event_handler->handle_events(events);
        process_events();
        ui->render();
        ui->last_frame_ms = ui->timer->get_ms_since_start();
    }
}
} // namespace tls
