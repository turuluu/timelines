#include <cassert>
#include <iostream>

#include "calendars.hpp"
#include "entities.hpp"
#include "utilities.hpp"

namespace tls
{
idx_t
to_index(time_point_t time_point)
{
    const auto index = util::limit<idx_t>(0, spec::max_bins - 1, spec::bins_split + time_point);
    assert(index >= 0 && index < spec::max_bins);
    return index;
}

time_point_t
to_interval(idx_t index)
{
    const time_point_t time_point = index - spec::bins_split;
    return time_point;
}

time_point_t
bin_limit(time_point_t time_point)
{
    return util::limit<time_point_t>(-spec::bins_split,
                                     spec::max_bins - spec::bins_split,
                                     time_point);
}
////////////////////////////////////////////////////////////////////////////////

intervals::~intervals() { std::cout << "DTOR " << __PRETTY_FUNCTION__ << "\n"; }

void
intervals::insert(const entity& e)
{
    for (auto i = to_index(e.interval.start); i < to_index(e.interval.end); ++i)
        ++interval_bins[i];
}

void
intervals::clear()
{
    std::fill(interval_bins.begin(), interval_bins.end(), 0);
}
} // namespace tls