#include "time.hpp"
#include "entities.hpp"
#include <cassert>

namespace tls
{
idx_t
to_index(time_point time_point)
{
    const auto index = utlz::limit<idx_t>(0,
                                          spec::max_bins - 1,
                                          spec::bins_split + time_point.time_since_epoch().count());
    assert(index < spec::max_bins);
    return index;
}

time_point
bin_limit(time_point time_point)
{
    using float_t = cosmic::clock::rep;
    // auto limited = utlz::limit<float_t>((float_t)-spec::bins_split,
                                        // (float_t)spec::max_bins - spec::bins_split,
                                        // time_point.time_since_epoch().count());
    auto limited = utlz::limit<float_t>(std::numeric_limits<float_t>::lowest(),
        std::numeric_limits<float_t>::max(),
        time_point.time_since_epoch().count());
    return cosmic::clock::time_point(cosmic::clock::duration(limited));
}

intervals::~intervals() { utlz::dbg("DTOR ", __PRETTY_FUNCTION__); }

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
}