#pragma once

#include "core.hpp"
#include "rendering.hpp"
#include "types.hpp"

namespace tls
{
struct application
{
    explicit application(core& core);

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

    void process_events();

    void loop();

    core& core;
    bool is_running{ true };
    events events;
    std::unique_ptr<event_handler_ifc> event_handler;
    std::unique_ptr<rendering_controller> ui;
};
} // namespace tls