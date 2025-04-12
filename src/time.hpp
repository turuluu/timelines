#pragma once

#include <utlz/all.hpp>

#include "spec.hpp"

namespace tls
{
namespace cosmic
{

struct clock
{
    using rep = double;
    using period = std::ratio<31556952>;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<clock>;

    static constexpr bool is_steady = false;

    // TODO: year 0 := Big bang
    static constexpr double big_bang_years_ago = 0;                      // 13.8e9;
    static constexpr double seconds_since_big_bang = big_bang_years_ago; // * 365.25 * 24 * 60 * 60;

    // Current point in relation to "cosmic history"
    static time_point now() noexcept { return time_point(duration(0)); }

    static double time_since_big_bang(const time_point& tp)
    {
        return tp.time_since_epoch().count();
    }

    static time_point from_years(double years)
    {
        return time_point(duration(years /* * 365.25 * 24 * 60 * 60*/));
    }

    // iso8601 includes 0, gregorian doesn't
    static time_point from_iso(double years)
    {
        return time_point(duration(years /** 365.25 * 24 * 60 * 60 + seconds_since_big_bang*/));
    }

    static time_point interpolate(const time_point& tp0, const time_point& tp1, float ratio)
    {
        auto d0 = tp0.time_since_epoch().count();
        auto d1 = tp1.time_since_epoch().count();
        auto interpolated = d0 + (d1 - d0) * ratio;

        auto x = time_point(duration(interpolated));
        return x;
    }

    static time_point midpoint(const time_point tp0, const time_point tp1)
    {
        return interpolate(tp0, tp1, 0.5);
    }
};

} // namespace cosmic

using clock = cosmic::clock;
using time_point = clock::time_point;

idx_t to_index(time_point time_point);
time_point_t to_interval(idx_t index);
time_point bin_limit(time_point time_point);

struct interval
{
    time_point start;
    time_point end;
};

static interval
new_relative_interval(const float xrel,
                      const interval& interval,
                      const int_pixels_t relative_max = spec::screen_w)
{
    auto duration = interval.end - interval.start;
    auto scale = (clock::rep)(duration.count()) / relative_max;
    int scaled_x = xrel * scale;
    return { interval.start + clock::duration(scaled_x), interval.end + clock::duration(scaled_x) };
}

static interval
new_scaled_interval(const float value,
                    const interval& interval,
                    const int_pixels_t mid_x = (spec::screen_w / 2),
                    const int_pixels_t relative_max = spec::screen_w)
{

    const float mid_point = 1.0f - ((float)mid_x / relative_max);
    float scale = std::pow(2.0f, -0.1f * value);
    scale = utlz::limit(0.125f, 8.0f, scale);

    const auto start = interval.start;
    const auto end = interval.end;
    const auto mid = clock::interpolate(start, end, mid_point);

    return { bin_limit(clock::interpolate(mid, start, scale)),
             bin_limit(clock::interpolate(mid, end, scale)) };
}

struct entity;
struct intervals
{
    intervals() = default;
    ~intervals();

    std::array<size_t, spec::max_bins> interval_bins{ 0 };

    void insert(const entity& e);
    void clear();
};
} // namespace tls