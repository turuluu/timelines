#include "rendering.hpp"

namespace tls
{
void
Renderer::set_controller(RenderingController* controller)
{
    this->controller = controller;
}

Core::Entities
Renderer::select_from(const Core::Entities& entities) const

{
    auto& years = controller->core.years;
    // TODO : views without copy, something that won't have invalidated entries..
    // Queus, lists?
    Core::Entities selected_entities;
    years.clear();
    for (auto& e : entities)
    {
        if (e.start_year < year_range.end && e.end_year > year_range.start)
        {
            selected_entities.push_back(e);
            years.insert(e);
        }
    }

    return selected_entities;
}
size_t
Renderer::entities_in_interval(int start, int end) const

{
    auto& years = controller->core.years;
    size_t max_entities_in_interval = 0;
    for (auto i = start; i < end; ++i)
        max_entities_in_interval =
          std::max(years.year_bins[year_to_index(i)], max_entities_in_interval);

    if (max_entities_in_interval == 0)
        std::cout << "no entities in time frame..\n";

    return max_entities_in_interval;
}
}