#pragma once

#include "../rendering.hpp"

namespace tls
{
struct Events
{
    bool should_quit{ false };
    bool toggle_renderer{ false };
    std::deque<MouseMove> mouse;
    std::deque<Sint32> wheel;
};
struct EventHandler
{
    virtual ~EventHandler() { printf("Event Handler DTOR\n"); }
    virtual void wait(size_t ms) const = 0;
    virtual void handle_events(Events& events) = 0;
};
struct Application
{
private:
    Application(EventHandler* specific_event_handler, RenderingController* controller_ptr)
      : event_handler(specific_event_handler)
      , controller(controller_ptr)
    {
    }

public:
    static Application own(EventHandler* specific_event_handler, RenderingController*
    controller_ptr)
    {
        return {specific_event_handler, controller_ptr};
    }

    ~Application()
    {
        printf("Application DTOR\n");
    }

    void process_events()
    {
        is_running = !events.should_quit;

        if (events.toggle_renderer)
        {
            controller->toggle_renderer();
            events.toggle_renderer = false;
        }

        Sint32 y_scroll_delta = 0;
        while (!events.wheel.empty())
        {
            y_scroll_delta += events.wheel.back();
            events.wheel.pop_back();
        }
        if (y_scroll_delta != 0)
        {
            int x = 0, y = 0;
            SDL_GetMouseState(&x, &y);
            controller->scroll_y(y_scroll_delta, x, y);
        }

        MouseMove mouse_move_delta = { 0, 0 };
        while (!events.mouse.empty())
        {
            mouse_move_delta.x += events.mouse.back().x;
            mouse_move_delta.y += events.mouse.back().y;
            events.mouse.pop_back();
        }
        if (mouse_move_delta.x != mouse_move_delta.y != 0)
            controller->button_left_drag(mouse_move_delta);
    }
    void loop()
    {
        while (is_running)
        {
            event_handler->handle_events(events);
            event_handler->wait(100);
            process_events();
        }
    }
    bool is_running{ true };
    Events events;
    std::unique_ptr<EventHandler> event_handler;
    std::unique_ptr<RenderingController> controller;
};
struct SdlEventHandler : EventHandler
{
    void wait(size_t ms) const override { SDL_Delay(ms); }

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
}