#include "rendering.hpp"
#include "entities.hpp"
#include "sdl/graphics.hpp"
#include "time.hpp"

namespace tls
{
renderer::renderer()
  : id(gen_id())
  , controller(nullptr)
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
    auto& lanes = controller->core.lanes;
    // TODO : views without copy, something that won't have invalidated entries..
    // Queus, lists?
    std::vector<std::reference_wrapper<const entity>> selected;
    lanes.clear();
    for (const auto& e : entities)
    {
        if (rendering_interval.start < e.interval.end && e.interval.start < rendering_interval.end)
        {
            selected.push_back(std::cref(e));
            lanes.insert(e);
        }
    }

    return selected;
}

size_t
renderer::lanes_in_interval(interval interval) const
{
    auto& lanes = controller->core.lanes;
    size_t lanes_count = 0;
    for (auto i = to_index(interval.start); i < to_index(interval.end); ++i)
        lanes_count = std::max(lanes.interval_bins[i], lanes_count);

    if (lanes_count == 0)
        std::cout << "no entities in time frame..\n";

    return lanes_count;
}

void
renderer::render_range(std::vector<entity>& entities, interval interval)
{
    assert(!entities.empty());

    const auto render_start = interval.start;
    const auto render_end = interval.end;
    assert(render_start <= render_end);

    auto selected_entities = select_from(entities);

    auto max_entities_in_interval = lanes_in_interval(interval);
    if (max_entities_in_interval == 0)
        return;

    style_info specs;
    specs.font = font;
    specs.font_size = font_size;
    specs.max_dimension = max_dim();
    specs.dimension = specs.max_dimension / max_entities_in_interval;

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
        const auto entity_start = e.get().interval.start;
        const auto entity_end = e.get().interval.end;

        auto start_bound = std::max(entity_start, render_start);
        specs.rect_start = to_index(start_bound) - bin_start;

        auto end_bound = std::min(entity_end, render_end);
        specs.rect_end = to_index(end_bound) - bin_start;

        // non const part
        specs.lane_index = lane_index(max_entities_in_interval, entity_start, entity_end);
        specs.color.r = e.get().id * colour_incr;
        specs.color.g = specs.color.g + colour_incr;

        style->render(specs, e);
    }

    SDL_RenderPresent(graphics::get().ren);
}

void
rendering_controller::move_viewport(mouse_move m, const float multiplier)
{
    if (!is_renderer_set())
        return;

    const auto multiplied_value =
      is_horizontal() ? ((float)-m.x) * multiplier : ((float)-m.y) * multiplier;

    auto& interval = get_renderer().rendering_interval;
    struct interval adjusted = new_relative_interval(multiplied_value, interval);
    interval = adjusted;
}

rect
stylist_v::lane_bounds(style_info s)
{
    rect r;
    r.x = 10 + (s.dimension * s.lane_index);
    r.y = s.rect_start * s.scale;
    r.h = (s.rect_end - s.rect_start) * s.scale;
    r.w = s.dimension;

    return r;
}

rect
stylist_v::text_bounds(style_info s)
{
    rect r;
    r.x = s.max_dimension + 10;
    r.y = s.rect_start * s.scale;
    r.h = s.font_size;
    r.w = s.max_dimension - 20;

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
                   specs.max_dimension + 20,
                   r.y + specs.font_size / 2);

    // outline
    SDL_SetRenderDrawColor(graphics::get().ren, 0xFF, 0xFF, 0xFF, 0x20);
    SDL_RenderRect(graphics::get().ren, &r);

    // fill
    SDL_SetRenderDrawColor(graphics::get().ren,
                           0x9F - (specs.color.r * 0.5f),
                           0x90 + (0xFF - specs.color.g) * 0.2f,
                           0xFF - (specs.color.b * 0.8f),
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
                           0x9F - (specs.color.r * 0.5f),
                           0x90 + (0xFF - specs.color.g) * 0.2f,
                           0xFF - (specs.color.b * 0.8f),
                           0x70);
    SDL_RenderFillRect(graphics::get().ren, &r);

    // outline
    SDL_SetRenderDrawColor(graphics::get().ren, 0xFF, 0xFF, 0xFF, 0x20);
    SDL_RenderRect(graphics::get().ren, &r);

    color text_color{ 255, 255 };
    auto offset = rect{ r.x, r.y - specs.font_size / 2, r.w, r.h };
    render_text_2(specs.font, &text_color, &offset, e.name.c_str(), specs.font_size);
}

rect
stylist_h::lane_bounds(style_info s)
{
    rect r;
    r.x = s.rect_start * s.scale;
    r.y = 10 + (s.dimension * s.lane_index);
    r.w = (s.rect_end - s.rect_start) * s.scale;
    r.h = s.dimension;

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
    static const auto zero_point = to_index(clock::from_iso(0));
    const auto start = interval.start.time_since_epoch().count();
    const auto end = interval.end.time_since_epoch().count();
    const auto length = end - start;
    int splits = length / 8.0f;
    splits = ((int)(splits / 10.0f)) * 10;
    splits = splits > 0 ? splits : 1;
    auto start_idx = to_index(interval.start);
    for (int i = start_idx; i < to_index(interval.end); ++i)
    {
        if (i % splits == 0)
        {
            SDL_SetRenderDrawColor(graphics::get().ren, 0x5F, 0x5F, 0x5F, 0x20);
            const int x = (i - start_idx) * scale_x;
            SDL_RenderLine(graphics::get().ren, x, 0, x, spec::screen_h);

            const int label_w = 50;
            const int label_h = 20;

            rect grid_label_bounds;
            grid_label_bounds.x = x - (label_w / 2);
            grid_label_bounds.y = spec::screen_h - label_h;
            grid_label_bounds.w = label_w;
            grid_label_bounds.h = label_h;

            color c{ 255, 255 };
            render_text_2(font,
                          &c,
                          &grid_label_bounds,
                          std::to_string(i - (int)zero_point).c_str(),
                          28);
        }
    }
}

rect
stylist_v_line::lane_bounds(style_info s)
{
    rect r;
    r.x = 10 + (s.dimension * s.lane_index);
    r.y = s.rect_start * s.scale;
    r.h = (s.rect_end - s.rect_start) * s.scale;
    r.w = s.dimension;
    return r;
}

rect
stylist_v_line::text_bounds(style_info s)
{
    rect r;
    r.x = s.max_dimension + 10;
    r.y = s.rect_start * s.scale;
    r.h = s.font_size;
    r.w = s.max_dimension - 20;
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
              point{ (float)specs.max_dimension + 20, r.y + specs.font_size / 2 },
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
    r.y = 10 + (bi.dimension * bi.lane_index);
    r.w = (bi.rect_end - bi.rect_start) * bi.scale;
    r.h = bi.dimension;
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
    float center_y = r.y + r.h / 2;

    // Lane line
    auto line_color = palette::teal_breaker[e.id % 5];
    draw_line(point{ r.x, center_y }, point{ r.x + r.w, center_y }, line_color);

    // Dots for caps
    color dot_color{ 255, 255 };
    float dot_radius = 3.0f;
    draw_filled_circle(r.x, center_y, dot_radius, dot_color);
    draw_filled_circle(r.x + r.w, center_y, dot_radius, dot_color);

    // Text
    auto offset = rect{ r.x, r.y - specs.font_size / 2, std::max(r.w, 100.0f), r.h };
    render_text_2(specs.font, &colors::baby_powder, &offset, e.name.c_str(), specs.font_size);
}

} // namespace tls