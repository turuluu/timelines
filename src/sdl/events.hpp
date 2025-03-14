#pragma once

#include <list>
#include <deque>

#include "../core.hpp"
#include "../types.hpp"
#include "../rendering.hpp"

namespace tls
{
// TODO : move these to core, inline sdl namespace, ...
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
struct handler
{};
struct application
{
    application(core& core)
      : core(core)
    {
    }

    ~application() { printf("Application DTOR\n"); }

    enum class handler_type: int
    {
        Event,
        UI,
    };

    // template <typename T, typename... Args>
    // T& make_handler(Args... args)
    // {
    // }
    //
    template <typename T, typename... Args>
    T& make(Args&... args)
    {
        if constexpr (std::is_base_of<event_handler_ifc, T>::value)
        {
            event_handler = std::make_unique<T>();
            return *dynamic_cast<T*>(event_handler.get());
        }
        if constexpr (std::is_base_of<rendering_controller, T>::value)
        {
            ui = std::make_unique<T>(std::forward<Args&...>(args...));
            return *ui;
        }
    }

    /*
    template <>
    IEventHandler& make<IEventHandler>()
    {
        return *event_handler;
    }

    template <>
    RenderingController& make<RenderingController>(Core& core)
    {
        ui = std::make_unique<RenderingController>(core);
        return *ui;
    }*/

    // placeholder
    std::list<std::unique_ptr<handler_type>> handlers;

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
    void loop()
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

    core& core;
    bool is_running{ true };
    events events;
    std::unique_ptr<event_handler_ifc> event_handler;
    std::unique_ptr<rendering_controller> ui;
};

#if USE_SDL
inline
#endif
namespace sdl
{
struct event_handler : event_handler_ifc
{
    void handle_events(events& events) override
    {
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
                case SDL_EVENT_QUIT:
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (e.motion.state == 1)
                    {
                        events.mouse.push_back({ e.motion.xrel, e.motion.yrel });
                    }
                    else if (e.motion.state == 4)
                    {
                        events.wheel.push_back(-e.motion.yrel);
                    }
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
                        events.wheel.push_back(e.wheel.y * wheel_y_mult);

                    break;
            }
        }
    }

    const float wheel_y_mult = 6.5f;
};

struct timer : timer_ifc
{
    [[nodiscard]] size_t get_ms_since_start() const override { return SDL_GetTicks(); }
    void wait_ms(size_t ms) const override { SDL_Delay(ms); }
};
}
}