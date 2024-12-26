#include <iostream>

#include "calendars.hpp"

namespace tls
{
tls::idx_t
year_to_index(tls::year_t year)
{
    const auto index = util::limit<tls::idx_t>(0, tls::MAX_BINS - 1, tls::BINS_SPLIT + year);
    assert(index >= 0 && index < tls::MAX_BINS);
    return index;
}

tls::year_t
index_to_year(tls::idx_t index)
{
    const tls::year_t year = index - tls::BINS_SPLIT;
    return year;
}

tls::year_t
year_limits(tls::year_t year)
{
    return util::limit<tls::year_t>(-tls::BINS_SPLIT, tls::MAX_BINS - tls::BINS_SPLIT, year);
}
////////////////////////////////////////////////////////////////////////////////

Years::~Years() { std::cout << "DTOR " << __PRETTY_FUNCTION__ << "\n"; }

void
Years::insert(Entity* e)
{
    for (auto i = year_to_index(e->start_year); i < year_to_index(e->end_year); ++i)
        ++year_bins[i];
}

void
Years::clear()
{
    std::fill(year_bins.begin(), year_bins.end(), 0);
}
}