#pragma once

#include "time_abstractions.hpp"

namespace tls
{

struct Core
{
    using Entities = std::vector<Entity>;
    void add(Entity&& entity);

    Entities data;
    Years years;
};

struct Filter
{

};
}