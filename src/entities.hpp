#pragma once

#include <set>
#include <vector>

#include "time.hpp"
#include "geometry.hpp"

namespace tls
{
static size_t ids = 0;


struct entity
{
    explicit entity(std::string _name);

    entity(std::string _name, time_point start, time_point end);

    ~entity();

    const size_t id;
    std::string name;
    interval interval;

    rect bounds;
    entity* parent = nullptr;
};

} // namespace tls