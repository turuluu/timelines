#pragma once

#include "../rendering.hpp"

namespace tls
{
struct Events
{
    bool should_quit{ false };
    bool toggle_renderer{ false };
    std::deque<MouseMove> mouse;
    std::deque<i32> wheel;
};


struct IEventHandler
{
    virtual ~IEventHandler() { printf("Event Handler DTOR\n"); }
    virtual void handle_events(Events& events) = 0;
};
struct Application
{
  private:
    Application(IEventHandler* specific_event_handler, RenderingController* controller_ptr)
      : event_handler(specific_event_handler)
      , ui(controller_ptr)
    {
    }

  public:
    static Application own(IEventHandler* specific_event_handler,
                           RenderingController* controller_ptr)
    {
        return { specific_event_handler, controller_ptr };
    }

    ~Application() { printf("Application DTOR\n"); }

    void process_events()
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
            int x = 0, y = 0;
            SDL_GetMouseState(&x, &y);
            ui->scroll_y(y_scroll_delta, x, y);
        }

        MouseMove mouse_move_delta = { 0, 0 };
        while (!events.mouse.empty())
        {
            mouse_move_delta.x += events.mouse.back().x;
            mouse_move_delta.y += events.mouse.back().y;
            events.mouse.pop_back();
        }
        if (mouse_move_delta.x != mouse_move_delta.y != 0)
            ui->button_left_drag(mouse_move_delta);
    }
    void loop()
    {
        ui->last_frame_ms = ui->timer->get_ms_since_start();
        while (is_running)
        {
            event_handler->handle_events(events);
            ui->wait_until_next_frame();
            process_events();
            ui->last_frame_ms = ui->timer->get_ms_since_start();
        }
    }
    bool is_running{ true };
    Events events;
    std::unique_ptr<IEventHandler> event_handler;
    std::unique_ptr<RenderingController> ui;
};

namespace sdl
{
struct EventHandler : IEventHandler
{
    void handle_events(Events& events) override
    {
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    break;
                case SDL_MOUSEMOTION:
                    if (e.motion.state == 1)
                    {
                        events.mouse.push_back({ e.motion.xrel, e.motion.yrel });
                    }
                    else if (e.motion.state == 4)
                    {
                        events.wheel.push_back(-e.motion.yrel);
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_q:
                            events.should_quit = true;
                            break;
                        case SDLK_t:
                            events.toggle_renderer = true;
                            break;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if (e.wheel.y != 0)
                        events.wheel.push_back(e.wheel.y * wheel_y_mult);

                    break;
            }
        }
    }

    const float wheel_y_mult = 6.5f;
};

struct Timer : ITimer
{
    [[nodiscard]] size_t get_ms_since_start() const override { return SDL_GetTicks(); }
    void wait_ms(size_t ms) const override { SDL_Delay(ms); }
};
}
}