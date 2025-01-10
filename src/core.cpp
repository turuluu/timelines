#include "core.hpp"
#include "entities.hpp"

namespace tls
{
void Core::add(Entity entity)
{
    data.emplace_back(std::move(entity));
}

void Core::add(Entity&& entity)
{
    data.emplace_back(std::forward<Entity>(entity));
}
}