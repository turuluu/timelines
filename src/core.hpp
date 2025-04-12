#pragma once

#include "time.hpp"

namespace tls
{
struct entity;
struct core
{
    using entities = std::vector<entity>;

    template<typename... Args>
    void add_entity(Args... args)
    {
        data.emplace_back(args...);
    }

    entities data;
    intervals lanes;
};

struct filter
{
    std::function<bool(entity)> predicate;
    std::vector<int> indices;
};
} // namespace tls