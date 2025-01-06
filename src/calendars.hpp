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

struct TimeInterface
{
};

struct JulianCalendar : TimeInterface
{
};

enum class Granularity : int
{
    Metric = -1,
    Seconds = 0,
    Minutes,
    Hours,
    Days,
    Weeks,
    Months,
    Years,
};

template<typename IntType>
struct TimePointT
{
    using type = IntType;
    type t{};
    type offset{};      // year 1 in gregorian calendar
    Granularity granularity{}; // seconds
};

// ISO 8601
struct Time : TimePointT<size_t>
{
    Time()
    {
        t = 1;
        granularity = Granularity::Years;
        offset = 13.8 * std::pow(10, 9);
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