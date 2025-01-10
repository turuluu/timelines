#pragma once

#include "calendars.hpp"

#include <array>

#include "entities.hpp"
#include "spec.hpp"

namespace tls
{
idx_t year_to_index(year_t year);
year_t index_to_year(idx_t index);
year_t year_limits(year_t year);

namespace calendar
{
enum class Type : unsigned
{
    Iso8601 = 0, // Normalized value
    Gregorian,
    Julian,
    Islamic,
    Hebrew,
    Coptic,
    SolarHijri,
    Bengali,
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
struct Base
{
    static constexpr auto type = Index;
};

//
struct JulianCalendar : Base<Type::Julian>
{
};
}

enum class Granularity : int
{
    Metric = -1, // metrics can be scaled in base 10
    Seconds = 0, // target normalization default for final
    Minutes,
    Hours,
    Days,
    Weeks,
    Months,
    Years, // wip normalization for experimentation
};

template<typename IntType, auto DefaultGranularity>
struct TimePointT
{
    using type = IntType;

    type t{};
    type offset{}; // year 0 in ISO8601 calendar
    Granularity granularity{ DefaultGranularity };
};

constexpr unsigned long start_of_the_universe_years_since_year_0 = 13'800'000'000UL;

// ISO 8601
struct Time : TimePointT<size_t, Granularity::Years>
{
    Time()
    {
        t = 1;
        offset = start_of_the_universe_years_since_year_0;
    }
};

struct TimeSeries
{
};

struct Years
{
    Years() = default;
    ~Years();

    std::array<size_t, spec::max_bins> year_bins{ 0 };

    void insert(const Entity& e);
    void clear();
};
}