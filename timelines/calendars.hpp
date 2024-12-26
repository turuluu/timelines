#pragma once

#include <array>
#include <cassert>

#include "details/utilities.h"
#include "entities.hpp"
#include "spec.hpp"

namespace tls
{
tls::idx_t year_to_index(tls::year_t year);
tls::year_t index_to_year(tls::idx_t index);
tls::year_t year_limits(tls::year_t year);

struct TimeInterface
{
};

struct JulianCalendar : public TimeInterface
{
};

struct Years
{
    Years() = default;
    ~Years();

    std::array<size_t, tls::spec::max_bins> year_bins{ 0 };

    void insert(Entity* e);
    void clear();
};
}