#pragma once

#include "time_abstractions.hpp"

namespace tls
{
struct core
{
    using entities = std::vector<entity>;

    template<typename... Args>
    void add_entity(Args... args)
    {
        data.emplace_back(args...);
    }

    entities data;
    intervals intervals;
};

struct Filter
{
    std::function<bool(entity)> predicate;
    std::vector<int> indices;
};
} // namespace tls