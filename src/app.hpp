#pragma once

#include "core.hpp"
#include "rendering.hpp"
#include "types.hpp"

namespace tls
{
// TODO : move these to core, inline sdl namespace, ...

struct handler
{
};
struct application
{
    application(core& core);

    ~application();

    enum class handler_type : int
    {
        Event,
        UI,
    };

    template<typename T, typename... Args>
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

    // placeholder
    std::list<std::unique_ptr<handler_type>> handlers;

    void process_events();

    void loop();

    core& core;
    bool is_running{ true };
    events events;
    std::unique_ptr<event_handler_ifc> event_handler;
    std::unique_ptr<rendering_controller> ui;
};

}