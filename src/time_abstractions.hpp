#pragma once

#include <cassert>
#include <deque>

#include "calendars.hpp"
#include "entities.hpp"
#include "sdl/graphics.hpp"
#include "utilities.hpp"
#include "spec.hpp"
#include "time_abstractions.hpp"

namespace tls
{

struct YearRange
{
    int start = 0;
    int end = 0;
};

static YearRange new_relative_year_range(const i32 xrel,
                        const YearRange* year_range,
                        const int_pixels_t relative_max = spec::screen_w)
{
    float scale = (float)(year_range->end - year_range->start) / relative_max;
    int scaled_x = xrel * scale;
    return { year_range->start+ scaled_x, year_range->end + scaled_x };
}

static YearRange new_scaled_year_range(const i32 value,
                      const YearRange* year_range,
                      const int_pixels_t mid_x = (spec::screen_w / 2),
                      const int_pixels_t relative_max = spec::screen_w)
{
    const float mid_point = 1.0f - ((float)mid_x / relative_max);
    constexpr float scale = 1e-2f;
    const float timescale = 1.0 - util::limit(-0.9f, 0.9f, scale * (float)value);
    const float start = year_range->start;
    const float mid = (year_range->start + year_range->end) * mid_point;
    const float end = year_range->end;

    const int new_start = year_limits((int)(((start - mid) * timescale) + mid));
    const int new_end = year_limits((int)(((end - mid) * timescale) + mid));
    return { new_start, new_end };
}
}