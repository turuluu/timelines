#include "rendering.hpp"
#include "entities.hpp"
#include "sdl/graphics.hpp"
#include "time_abstractions.hpp"

namespace tls
{
renderer::renderer()
  : id(gen_id())
  , font_size(36)
  , font{ get_title_font(font_size) }
{
}

renderer::~renderer() { destroy_font(font); }

void
renderer::set_controller(rendering_controller* controller)
{
    this->controller = controller;
}

std::vector<std::reference_wrapper<const entity>>
renderer::select_from(const core::entities& entities) const
{
    auto& intervals = controller->core.intervals;
    // TODO : views without copy, something that won't have invalidated entries..
    // Queus, lists?
    std::vector<std::reference_wrapper<const entity>> selected_entities;
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
renderer::entities_in_interval(interval interval) const
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
renderer::render_range(std::vector<entity>& entities, interval interval)
{
    assert(!entities.empty());

    const auto render_start = interval.start;
    const auto render_end = interval.end;
    assert(render_start <= render_end);

    auto selected_entities = select_from(entities);

    auto max_entities_in_interval = entities_in_interval(interval);
    if (max_entities_in_interval == 0)
        return;

    style_info specs;
    specs.font = font;
    specs.font_size = font_size;
    specs.max_d = max_dim();
    specs.d = specs.max_d / max_entities_in_interval;

    const auto bin_start = to_index(render_start);
    const auto bin_end = to_index(render_end);
    const auto bin_len = utlz::limit<int>(0, spec::max_bins, bin_end - bin_start);
    specs.scale = get_scale(bin_len);

    clear();
    std::fill(lanes.begin(), lanes.end(), std::numeric_limits<uint8_t>::max());
    draw_grid(rendering_interval, specs.scale);

    u8 colour_incr = 255 / entities.size();
    for (const auto& e : selected_entities)
    {
        const int entity_start = e.get().interval.start;
        const int entity_end = e.get().interval.end;

        auto start_bound = std::max(entity_start, render_start);
        specs.rect_start = to_index(start_bound) - bin_start;

        auto end_bound = std::min(entity_end, render_end);
        specs.rect_end = to_index(end_bound) - bin_start;

        // non const part
        specs.lane_index = lane(max_entities_in_interval, entity_start, entity_end);
        specs.color.fill = e.get().id * colour_incr;
        specs.color.border = specs.color.fill + colour_incr;

        style->render(specs, e);
    }

    SDL_RenderPresent(graphics::get().ren);
}

void
rendering_controller::button_left_drag(mouse_move m, const float multiplier)
{
    if (!is_renderer_set())
        return;

    const i32 multiplied_value =
      is_horizontal() ? ((float)-m.x) * multiplier : ((float)-m.y) * multiplier;

    auto& interval = get_renderer().rendering_interval;
    struct interval adjusted = new_relative_interval(multiplied_value, interval);
    interval = adjusted;
}

rect
stylist_v::lane_bounds(style_info s)
{
    rect r;
    r.x = 10 + (s.d * s.lane_index);
    r.y = s.rect_start * s.scale;
    r.h = (s.rect_end - s.rect_start) * s.scale;
    r.w = s.d;

    return r;
}

rect
stylist_v::text_bounds(style_info s)
{
    rect r;
    r.x = s.max_d + 10;
    r.y = s.rect_start * s.scale;
    r.h = s.font_size;
    r.w = s.max_d - 20;

    return r;
}

void
stylist_v::render(style_info specs, const entity& e)
{
    auto r = lane_bounds(specs);

    // indicator line connecting text and lane box
    SDL_RenderLine(graphics::get().ren,
                   r.x + r.w,
                   r.y + specs.font_size / 2,
                   specs.max_d + 20,
                   r.y + specs.font_size / 2);

    // outline
    SDL_SetRenderDrawColor(graphics::get().ren, 0xFF, 0xFF, 0xFF, 0x20);
    SDL_RenderRect(graphics::get().ren, &r);

    // fill
    SDL_SetRenderDrawColor(graphics::get().ren,
                           0x9F - (specs.color.fill * 0.5f),
                           0x90 + (0xFF - specs.color.fill) * 0.2f,
                           0xFF - (specs.color.fill * 0.8f),
                           0x70);
    SDL_RenderFillRect(graphics::get().ren, &r);

    auto rt = text_bounds(specs);
    color text_color{ 255, 255 };
    render_text(specs.font, &text_color, &rt, e.name.c_str(), specs.font_size);
}

vertical::vertical()
{
    utlz::dbg("CTOR ", __PRETTY_FUNCTION__);
    orientation = orientation::vertical;

    assert(font != nullptr);

    // TODO : clean up to their own wrapper
    SDL_SetRenderDrawColor(graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(graphics::get().ren);

    lanes.resize(spec::max_bins);

    style = std::make_unique<stylist_v_line>();
}

int
vertical::max_dim()
{
    return spec::screen_w / 2;
}

double
vertical::get_scale(double bin_len)
{
    return spec::screen_w / bin_len;
}

int
horizontal::max_dim()
{
    return spec::screen_h - 80;
}

double
horizontal::get_scale(double bin_len)
{
    return spec::screen_w / bin_len;
}

void
stylist_h::render(style_info specs, const entity& e)
{
    auto r = lane_bounds(specs);

    // fill
    SDL_SetRenderDrawColor(graphics::get().ren,
                           0x9F - (specs.color.fill * 0.5f),
                           0x90 + (0xFF - specs.color.fill) * 0.2f,
                           0xFF - (specs.color.fill * 0.8f),
                           0x70);
    SDL_RenderFillRect(graphics::get().ren, &r);

    // outline
    SDL_SetRenderDrawColor(graphics::get().ren, 0xFF, 0xFF, 0xFF, 0x20);
    SDL_RenderRect(graphics::get().ren, &r);

    color text_color{ 255, 255 };
    auto offset = rect{r.x, r.y - specs.font_size / 2, r.w, r.h};
    render_text_2(specs.font, &text_color, &offset, e.name.c_str(), specs.font_size);
}

rect
stylist_h::lane_bounds(style_info bi)
{
    rect r;
    r.x = bi.rect_start * bi.scale;
    r.y = 10 + (bi.d * bi.lane_index);
    r.w = (bi.rect_end - bi.rect_start) * bi.scale;
    r.h = bi.d;

    return r;
}

rect
stylist_h::text_bounds(style_info bi)
{
    return {};
}

horizontal::horizontal()
{
    utlz::dbg("CTOR ", __PRETTY_FUNCTION__);

    assert(font != nullptr);

    SDL_SetRenderDrawColor(graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(graphics::get().ren);

    lanes.resize(spec::max_bins);

    style = std::make_unique<stylist_h_line>();
}

void
horizontal::draw_grid(interval interval, const double scale_x) const
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
            SDL_SetRenderDrawColor(graphics::get().ren, 0x5F, 0x5F, 0x5F, 0x20);
            const int x = (to_index(i) - start_idx) * scale_x;
            SDL_RenderLine(graphics::get().ren, x, 0, x, spec::screen_h);

            const int label_w = 50;
            const int label_h = 20;

            rect grid_label_bounds;
            grid_label_bounds.x = x - (label_w / 2);
            grid_label_bounds.y = spec::screen_h - label_h;
            grid_label_bounds.w = label_w;
            grid_label_bounds.h = label_h;

            color c{ 255, 255 };
            render_text_2(font, &c, &grid_label_bounds, std::to_string(i).c_str(), 28);
        }
    }
}

rect
stylist_v_line::lane_bounds(style_info s)
{
    rect r;
    r.x = 10 + (s.d * s.lane_index);
    r.y = s.rect_start * s.scale;
    r.h = (s.rect_end - s.rect_start) * s.scale;
    r.w = s.d;
    return r;
}

rect
stylist_v_line::text_bounds(style_info s)
{
    rect r;
    r.x = s.max_d + 10;
    r.y = s.rect_start * s.scale;
    r.h = s.font_size;
    r.w = s.max_d - 20;
    return r;
}

void
stylist_v_line::render(style_info specs, const entity& e)
{
    auto r = lane_bounds(specs);

    // timeline-line
    draw_lane_line(specs, r);
    draw_lane_dots(specs, r);

    // indicator line connecting text and lane box
    draw_line(point{ r.x + r.w / 2 + 1.0f, r.y + specs.font_size / 2 },
              point{ (float)specs.max_d + 20, r.y + specs.font_size / 2 },
              color{ 0x77, 0x77 });

    auto rt = text_bounds(specs);
    color text_color{ 255, 255 };
    render_text(specs.font, &text_color, &rt, e.name.c_str(), specs.font_size);
}

void
stylist_v_line::draw_lane_line(style_info specs, rect bounds)
{
    color line_color{ 255, 255 };
    float center_x = bounds.x + bounds.w / 2;
    draw_line(point{ center_x, bounds.y }, point{ center_x, bounds.y + bounds.h }, line_color);
}

void
stylist_v_line::draw_lane_dots(style_info specs, rect bounds)
{
    color dot_color{ 255, 255 };
    float center_x = bounds.x + bounds.w / 2;
    float dot_radius = 3.0f;

    draw_filled_circle(center_x, bounds.y, dot_radius, dot_color);
    draw_filled_circle(center_x, bounds.y + bounds.h, dot_radius, dot_color);
}

rect
stylist_h_line::lane_bounds(style_info bi)
{
    rect r;
    r.x = bi.rect_start * bi.scale;
    r.y = 10 + (bi.d * bi.lane_index);
    r.w = (bi.rect_end - bi.rect_start) * bi.scale;
    r.h = bi.d;
    return r;
}

rect
stylist_h_line::text_bounds(style_info bi)
{
    return {};
}

void
stylist_h_line::render(style_info specs, const entity& e)
{
    auto r = lane_bounds(specs);

    draw_lane_line(specs, r);
    draw_lane_dots(specs, r);

    color text_color{ 255, 255 };
    auto offset = rect{r.x, r.y - specs.font_size / 2, r.w, r.h};
    render_text_2(specs.font, &text_color, &offset, e.name.c_str(), specs.font_size);
}

void
stylist_h_line::draw_lane_line(style_info specs, rect bounds)
{
    color line_color{ 255, 255 };
    float center_y = bounds.y + bounds.h / 2;
    draw_line(point{ bounds.x, center_y }, point{ bounds.x + bounds.w, center_y }, line_color);
}

void
stylist_h_line::draw_lane_dots(style_info specs, rect bounds)
{
    color dot_color{ 255, 255 };
    float center_y = bounds.y + bounds.h / 2;
    float dot_radius = 3.0f;

    draw_filled_circle(bounds.x, center_y, dot_radius, dot_color);
    draw_filled_circle(bounds.x + bounds.w, center_y, dot_radius, dot_color);
}
} // namespace tls