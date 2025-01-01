#pragma once

#include <array>

#include "entities.hpp"
#include "spec.hpp"

namespace tls
{
idx_t year_to_index(tls::year_t year);
year_t index_to_year(tls::idx_t index);
year_t year_limits(tls::year_t year);

struct TimeInterface
{
};

struct JulianCalendar : TimeInterface
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