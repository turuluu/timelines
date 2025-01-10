#include <iostream>
#include <cassert>

#include "calendars.hpp"
#include "utilities.hpp"

namespace tls
{
idx_t
year_to_index(year_t year)
{
    const auto index = util::limit<idx_t>(0, spec::max_bins - 1, spec::bins_split + year);
    assert(index >= 0 && index < spec::max_bins);
    return index;
}

year_t
index_to_year(idx_t index)
{
    const year_t year = index - spec::bins_split;
    return year;
}

year_t
year_limits(year_t year)
{
    return util::limit<year_t>(-spec::bins_split, spec::max_bins - spec::bins_split, year);
}
////////////////////////////////////////////////////////////////////////////////

Years::~Years() { std::cout << "DTOR " << __PRETTY_FUNCTION__ << "\n"; }

void
Years::insert(const Entity& e)
{
    for (auto i = year_to_index(e.start_year); i < year_to_index(e.end_year); ++i)
        ++year_bins[i];
}

void
Years::clear()
{
    std::fill(year_bins.begin(), year_bins.end(), 0);
}
}