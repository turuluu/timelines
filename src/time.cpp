#include "time.hpp"
#include "entities.hpp"
#include <cassert>

namespace tls
{
idx_t
to_index(time_point time_point)
{
    double years = time_point.time_since_epoch().count();

    // Map the years to a bin index using max_bins/2 as the center point to allow for negative years
    double center = spec::max_bins / 2.0;
    double scale = center / 10000.0;
    double raw_index = center + (years * scale);

    idx_t index = utlz::limit<idx_t>(0, spec::max_bins - 1, static_cast<idx_t>(raw_index));
    return index;
}

time_point
bin_limit(time_point time_point)
{
    using float_t = cosmic::clock::rep;
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
} // namespace tls