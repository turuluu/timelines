#pragma once

#include "time_abstractions.hpp"

namespace tls
{

struct Core
{
    using Entities = std::vector<Entity>;
    template <typename... Args>
    void add_entity(Args... args)
    {
        data.emplace_back(args...);
    }

    Entities data;
    Years years;
};

struct Filter
{
    std::function<bool(Entity)> predicate;
    std::vector<int> indices;
};
}