#include <iostream>
#include <cassert>

#include "entities.hpp"
#include "calendars.hpp"
#include "utilities.hpp"

namespace tls
{
idx_t
to_index(time_point_t year)
{
    const auto index = util::limit<idx_t>(0, spec::max_bins - 1, spec::bins_split + year);
    assert(index >= 0 && index < spec::max_bins);
    return index;
}

time_point_t
to_interval(idx_t index)
{
    const time_point_t year = index - spec::bins_split;
    return year;
}

time_point_t
bin_limit(time_point_t time_point)
{
    return util::limit<time_point_t>(-spec::bins_split, spec::max_bins - spec::bins_split, time_point);
}
////////////////////////////////////////////////////////////////////////////////

Intervals::~Intervals() { std::cout << "DTOR " << __PRETTY_FUNCTION__ << "\n"; }

void
Intervals::insert(const Entity& e)
{
    for (auto i = to_index(e.interval.start); i < to_index(e.interval.end); ++i)
        ++interval_bins[i];
}

void
Intervals::clear()
{
    std::fill(interval_bins.begin(), interval_bins.end(), 0);
}
}