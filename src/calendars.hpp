#pragma once

#include <array>

#include "calendars.hpp"
#include "spec.hpp"

namespace tls
{
idx_t to_index(time_point_t time_point);
time_point_t to_interval(idx_t index);
time_point_t bin_limit(time_point_t time_point);

namespace calendar
{
enum class calendar_type : unsigned
{
    io8601 = 0, // Normalized value
    gregorian,
    julian,
    islamic,
    hebrew,
    coptic,
    solar_hijri,
    bengali,
};

/**
 * Iso8601 ~ Gregorian (year 0 = Gregorian year -1 i.e. 1BC)
 * Gregorian calendar	January 9, 2025
 * Islamic calendar	9 Rajab, 1446 AH
 * Hebrew calendar	9 Tevet, AM 5785
 * Coptic calendar	Tobi 1, 1741 AM
 * Solar Hijri calendar	20 Dey, 1403 SH
 * Bengali calendar	Poush 26, 1431 BS
 * Julian calendar	27 December 2024
 * Byzantine calendar	9 January 7533
 */
template<auto Index>
struct calendar_base
{
    static constexpr auto type = Index;
};

//
struct julian_calendar : calendar_base<calendar_type::julian>
{
};
} // namespace calendar

enum class granularity : int
{
    metric = -1, // metrics can be scaled in base 10
    seconds = 0, // target normalization default for final
    minutes,
    hours,
    days,
    weeks,
    months,
    years, // wip normalization for experimentation
};

template<typename IntType, auto DefaultGranularity>
struct time_point_base
{
    using type = IntType;

    type t{};
    type offset{}; // year 0 in ISO8601 calendar
    granularity granularity{ DefaultGranularity };
};

constexpr unsigned long start_of_the_universe_years_since_year_0 = 13'800'000'000UL;

// ISO 8601
struct time : time_point_base<size_t, granularity::years>
{
    time()
    {
        t = 1;
        offset = start_of_the_universe_years_since_year_0;
    }
};

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