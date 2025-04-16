#pragma once

#include "time.hpp"
#include <vector>

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
    lanes lanes;
};
} // namespace tls