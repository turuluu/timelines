#include "rendering.hpp"
#include "entities.hpp"
#include "time_abstractions.hpp"

namespace tls
{
void
Renderer::set_controller(RenderingController* controller)
{
    this->controller = controller;
}

std::vector<std::reference_wrapper<const Entity>>
Renderer::select_from(const Core::Entities& entities) const
{
    auto& intervals = controller->core.intervals;
    // TODO : views without copy, something that won't have invalidated entries..
    // Queus, lists?
    std::vector<std::reference_wrapper<const Entity>> selected_entities;
    intervals.clear();
    for (auto& e : entities)
    {
        if (e.interval.start < rendering_interval.end && e.interval.end > rendering_interval.start)
        {
            selected_entities.emplace_back(e);
            intervals.insert(e);
        }
    }

    return selected_entities;
}

size_t
Renderer::entities_in_interval(Interval interval) const
{
    auto& intervals = controller->core.intervals;
    size_t max_entities_in_interval = 0;
    for (auto i = interval.start; i < interval.end; ++i)
        max_entities_in_interval =
          std::max(intervals.interval_bins[to_index(i)], max_entities_in_interval);

    if (max_entities_in_interval == 0)
        std::cout << "no entities in time frame..\n";

    return max_entities_in_interval;
}

void
HBoxStyle::render(SDL_Rect r, const Entity& e, u8 colour_border, u8 colour_fill) const
{
    // fill
    SDL_SetRenderDrawColor(Graphics::get().ren,
                           0x9F - (colour_fill * 0.5f),
                           0x90 + (0xFF - colour_fill) * 0.2f,
                           0xFF - (colour_fill * 0.8f),
                           0x70);
    SDL_RenderFillRect(Graphics::get().ren, &r);

    // outline
    SDL_SetRenderDrawColor(Graphics::get().ren, 0xFF, 0xFF, 0xFF, 0x20);

    SDL_RenderDrawRect(Graphics::get().ren, &r);
    SDL_Color color{ 255, 255, 255, 0x80 };
    render_text_2(font, &color, &r, e.name.c_str(), font_size);
}

void
Vertical::render_range(Core::Entities& _entities, Interval interval)
{
    assert(!_entities.empty());

    assert(interval.start <= interval.end);

    auto maxW = spec::screen_w / 2;

    auto selected_entities = select_from(_entities);

    auto max_entities_in_interval = entities_in_interval(interval);
    if (max_entities_in_interval == 0)
        return;

    auto w = maxW / max_entities_in_interval;

    clear();

    u8 colour_incr = 255 / _entities.size();

    const auto bin_start = to_index(interval.start);
    const auto bin_end = to_index(interval.end);
    const auto bin_len = util::limit<int>(0, spec::max_bins, bin_end - bin_start);
    const double scale_y = spec::screen_h / (double)bin_len;

    std::fill(lanes.begin(), lanes.end(), std::numeric_limits<uint8_t>::max());

    for (const auto& e : selected_entities)
    {
        const int entity_start = e.get().interval.start;
        const int entity_end = e.get().interval.end;

        auto start_bound = std::max(entity_start, interval.start);
        const int rect_start_y = to_index(start_bound) - bin_start;

        auto end_bound = std::min(entity_end, interval.end);
        const int rect_end_y = to_index(end_bound) - bin_start;

        // non const part
        const size_t lane_index = lane(max_entities_in_interval, entity_start, entity_end);

        SDL_Rect r;
        r.x = 10 + (w * lane_index);
        r.y = rect_start_y * scale_y;
        r.h = (rect_end_y - rect_start_y) * scale_y;
        r.w = w;
        const u8 colour_fill = e.get().id * colour_incr;
        const u8 colour_border = colour_fill + colour_incr;

        style->render(r, e, colour_border, colour_fill);
        SDL_RenderDrawLine(Graphics::get().ren,
                           r.x + r.w,
                           r.y + font_size / 2,
                           spec::screen_w / 2 + 20,
                           r.y + font_size / 2);

        SDL_Rect rt;
        rt.x = spec::screen_w / 2 + 10;
        rt.y = rect_start_y * scale_y;
        rt.h = font_size;
        rt.w = spec::screen_w / 2 - 20;

        SDL_Color color{ 255, 255, 255, 0xDD };
        render_text(font, &color, &rt, e.get().name.c_str(), font_size);
    }

    SDL_RenderPresent(Graphics::get().ren);
}

void
Horizontal::render_range(Core::Entities& _entities, Interval interval)
{
    const auto render_start = interval.start;
    const auto render_end = interval.end;

    assert(render_start <= render_end);
    auto max_h = spec::screen_h - 80;

    auto selected_entities = select_from(_entities);

    auto max_entities_in_interval = entities_in_interval(rendering_interval);
    if (max_entities_in_interval == 0)
        return;

    auto h = max_h / max_entities_in_interval;

    clear();

    assert(_entities.size() > 0);
    u8 colour_incr = 255 / _entities.size();

    const auto render_start_x = to_index(render_start);
    const auto render_end_x = to_index(render_end);
    const auto time_point = util::limit<int>(0, spec::max_bins, render_end_x - render_start_x);
    const double xScale = spec::screen_w / (double)time_point;

    std::fill(lanes.begin(), lanes.end(), std::numeric_limits<uint8_t>::max());

    for (const auto& e : selected_entities)
    {
        draw_grid(rendering_interval, xScale);

        const int entity_start = e.get().interval.start;
        const int entity_end = e.get().interval.end;

        auto bound_start = std::max(entity_start, render_start);
        const int rect_start_x = to_index(bound_start) - render_start_x;

        auto bound_end = std::min(entity_end, render_end);
        const int rect_end_x = to_index(bound_end) - render_start_x;

        // non const part
        const size_t lane_idx = lane(max_entities_in_interval, entity_start, entity_end);

        SDL_Rect r;
        r.x = rect_start_x * xScale;
        r.y = 10 + (h * lane_idx);
        r.w = (rect_end_x - rect_start_x) * xScale;
        r.h = h;

        const u8 fill_colour = e.get().id * colour_incr;
        const u8 border_colour = fill_colour + colour_incr;

        style->render(r, e, border_colour, fill_colour);
    }

    SDL_RenderPresent(Graphics::get().ren);
}

void
Horizontal::draw_grid(Interval interval, const double scale_x) const
{
    const int length = interval.end - interval.start;
    int splits = length / 8.0f;
    splits = ((int)(splits / 10.0f)) * 10;
    splits = splits > 0 ? splits : 1;
    auto start_idx = to_index(interval.start);
    for (int i = interval.start; i < interval.end; ++i)
    {
        if (i % splits == 0)
        {
            SDL_SetRenderDrawColor(Graphics::get().ren, 0x5F, 0x5F, 0x5F, 0x20);
            const int x = (to_index(i) - start_idx) * scale_x;
            SDL_RenderDrawLine(Graphics::get().ren, x, 0, x, spec::screen_h);

            const int label_w = 50;
            const int label_h = 20;

            SDL_Rect grid_label_bounds;
            grid_label_bounds.x = x - (label_w / 2);
            grid_label_bounds.y = spec::screen_h - label_h;
            grid_label_bounds.w = label_w;
            grid_label_bounds.h = label_h;

            SDL_Color c{ 255, 255, 255, 100 };
            render_text_2(font, &c, &grid_label_bounds, std::to_string(i).c_str(), 28);
        }
    }
}
}