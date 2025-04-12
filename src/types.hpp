#pragma once
#include <deque>

#include "geometry.hpp"

namespace tls
{
struct color
{
    u8 r = 255;
    u8 g = 255;
    u8 b = 255;
    u8 a = 255;
};
namespace colors
{
constexpr color night{ 16, 16, 12, 255 };
constexpr color dark_magenta{ 26, 24, 30, 255 };
constexpr color space_cadet{ 32, 32, 79, 255 };
constexpr color federal_blue{ 0, 4, 67, 255 };
constexpr color sky_blue{ 119, 194, 212, 255 };
constexpr color hot_pink{ 247, 111, 173, 255 };
constexpr color peach{ 253, 191, 155, 255 };
constexpr color honeydew{ 232, 248, 238, 255 };
constexpr color sky_magenta{ 172, 103, 168, 255 };
constexpr color lapis_lazuli{ 51, 101, 138 };

constexpr color baby_powder{ 255, 255, 250 };
constexpr color caribbean_current{ 13, 92, 99 };
constexpr color verdigris{ 68, 161, 160 };
constexpr color teal{ 41, 127, 130 };
constexpr color tiffany_blue{ 120, 205, 215 };
constexpr color teal_lite{ 36, 123, 123 };
} // namespace colors
namespace palette
{
constexpr std::array<const color, 5> giant_goldfish{ color{ 105, 210, 231 },
                                                     { 167, 219, 216 },
                                                     { 224, 228, 204 },
                                                     { 243, 134, 48 },
                                                     { 250, 105, 0 } };
constexpr std::array<const color, 5> color_fireworks{ colors::sky_blue,
                                                      colors::hot_pink,
                                                      colors::peach,
                                                      colors::honeydew,
                                                      colors::sky_magenta };
constexpr std::array<const color, 5> teal_breaker{ colors::caribbean_current,
                                                   colors::verdigris,
                                                   colors::teal,
                                                   colors::tiffany_blue,
                                                   colors::teal_lite };
}; // namespace palette

struct mouse_move
{
    float x;
    float y;
};

struct wheel_move
{
    float mouse_x;
    float mouse_y;
    float wheel_delta;
};

struct events
{
    bool should_quit{ false };
    bool toggle_renderer{ false };
    std::deque<mouse_move> move;
    std::deque<wheel_move> zoom;
};

struct event_handler_ifc
{
    virtual ~event_handler_ifc() { printf("Event Handler DTOR\n"); }
    virtual void handle_events(events& events) = 0;
};

struct timer_ifc
{
    virtual ~timer_ifc() = default;
    virtual void wait_ms(size_t delay_ms) const = 0;

    /**
     * time delta since initiation
     */
    [[nodiscard]] virtual size_t get_ms_since_start() const = 0;
};
} // namespace tls