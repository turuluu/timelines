#pragma once

#include <set>
#include <vector>

#include "time_abstractions.hpp"

namespace tls
{
static size_t ids = 0;

struct dimensions
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

struct entity
{
    explicit entity(std::string _name);

    entity(std::string _name, int _start, int _end);

    ~entity();

    enum class property : unsigned
    {
        none = 0x0,
        has_name_and_id = 0x1,
        has_start = 0x2,
        has_end = 0x4
    };

    const size_t id;
    std::string name;
    interval interval;
    property properties = property::none;

    dimensions bounds;
    entity* parent = nullptr;
};
} // namespace tls