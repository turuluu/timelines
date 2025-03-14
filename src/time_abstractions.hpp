#pragma once


#include "calendars.hpp"
#include "spec.hpp"
#include "utilities.hpp"

namespace tls
{

struct interval
{
    int start = 0;
    int end = 0;
};

static interval
new_relative_interval(const float xrel,
                      const interval& interval,
                      const int_pixels_t relative_max = spec::screen_w)
{
    float scale = (float)(interval.end - interval.start) / relative_max;
    int scaled_x = xrel * scale;
    return { interval.start + scaled_x, interval.end + scaled_x };
}

static interval
new_scaled_interval(const float value,
                      const interval& interval,
                      const int_pixels_t mid_x = (spec::screen_w / 2),
                      const int_pixels_t relative_max = spec::screen_w)
{
    const float mid_point = 1.0f - ((float)mid_x / relative_max);
    constexpr float scale = 1e-2f;
    const float timescale = 1.0 - util::limit(-0.9f, 0.9f, scale * (float)value);
    const float start = interval.start;
    const float mid = (interval.start + interval.end) * mid_point;
    const float end = interval.end;

    const int new_start = bin_limit((int)(((start - mid) * timescale) + mid));
    const int new_end = bin_limit((int)(((end - mid) * timescale) + mid));
    return { new_start, new_end };
}
}