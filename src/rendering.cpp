#include "rendering.hpp"
#include "entities.hpp"
#include "sdl/graphics.hpp"
#include "time.hpp"
#include "ui/components.hpp"

namespace tls
{
////////////////////////////////////////////////////////////////////////////////
// rendering_controller

gui::gui(struct core& core)
  : core(core)
{
    base.add<ui::time_indicator>({});
    utlz::dbg("component: ", base.components[0]->name());
}

bool
gui::is_horizontal() const
{
    return get_renderer().orientation == renderer::orientation::horizontal;
}

bool
gui::is_vertical() const
{
    return get_renderer().orientation == renderer::orientation::vertical;
}

void
gui::set_refresh_rate(size_t refresh_rate)
{
    frame_interval_ms = 1000 / refresh_rate;
}

void
gui::wait_until_next_frame() const
{
    auto time_delta_ms = timer->get_ms_since_start();
    auto elapsed_ms = time_delta_ms - last_frame_ms;
    if (elapsed_ms >= frame_interval_ms)
        return;
    else
        timer->wait_ms(frame_interval_ms - elapsed_ms);
}

void
gui::zoom(wheel_move scroll)
{
    if (renderer_idx < 0)
        return;

    // TODO : zoom for the vertical layout / Now it's a normal scroll
    auto screen_dim = is_horizontal() ? spec::screen_w : -spec::screen_h / 2;
    auto& interval = get_renderer().rendering_interval;
    auto focus_point = screen_dim + (is_horizontal() ? -scroll.mouse_x : scroll.mouse_y);
    struct interval timescaled =
      new_scaled_interval(scroll.wheel_delta, interval, focus_point, screen_dim);

    interval = timescaled;


}

void
gui::refresh()
{
    for (const auto& handle : base.slots)
    {
        auto index = handle.index;
        ui::context ctx { { 0, 0, spec::screen_w, spec::screen_h }, mouse, this };

        base.components[index]->layout(ctx);
    }

    ui::context ctx { {0, 0, spec::screen_w, spec::screen_h}, mouse, this };
    for (const auto& handle : base.slots)
        base.components[handle.index]->refresh(ctx);
}

void
gui::render()
{
    clear();

    for (const auto& handle : base.slots)
        base.components[handle.index]->draw({});

    get_renderer().render_range(core.data, get_renderer().rendering_interval);
}

bool
gui::is_renderer_set() const
{
    return renderer_idx >= 0;
}

void
gui::toggle_renderer()
{
    toggle = !toggle;
    renderer_idx = (int)toggle;
    get_renderer().render_range(core.data, renderer_container[!toggle]->rendering_interval);
}

void
gui::set_current(renderer& renderer_ref)
{
    int i = 0;
    while (i < renderer_container.size() && *renderer_container[i] != renderer_ref)
        ++i;

    assert(i < renderer_container.size());
    renderer_idx = i;
}

const renderer&
gui::get_renderer() const
{
    assert(is_renderer_set());
    return *renderer_container[renderer_idx];
}

renderer&
gui::get_renderer()
{
    assert(is_renderer_set());
    return *renderer_container[renderer_idx];
}

void
gui::mouse_move(struct mouse_move m, const float multiplier)
{
    if (!is_renderer_set())
        return;

    mouse.x = m.x;
    mouse.y = m.y;
}

void
gui::move_viewport(struct mouse_move m, const float multiplier)
{
    if (!is_renderer_set())
        return;

    const auto multiplied_value =
      is_horizontal() ? ((float)-m.x) * multiplier : ((float)-m.y) * multiplier;

    auto& interval = get_renderer().rendering_interval;
    struct interval adjusted = new_relative_interval(multiplied_value, interval);
    interval = adjusted;
}

////////////////////////////////////////////////////////////////////////////////
// renderer
renderer::renderer()
  : id(gen_id())
  , controller(nullptr)
  , font_size(36)
  , font{ get_title_font(font_size) }
{
}

renderer::~renderer() { destroy_font(font); }

void
renderer::set_controller(gui* controller)
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
        lanes_count = std::max(lanes.lane_bins[i], lanes_count);

    if (lanes_count == 0)
        std::cout << "no entities in time frame..\n";

    return lanes_count;
}

size_t
renderer::lane_index(size_t max_entities_in_interval, time_point start, time_point end)
{
    size_t lane = 0;
    for (lane = 0; lane < max_entities_in_interval; ++lane)
    {
        // Find empty lane
        if (std::all_of(lanes.begin() + to_index(start),
                        lanes.begin() + to_index(end),
                        [&lane](const auto& bits) { return !bits[lane]; }))
        {
            // mark lane
            for (auto vbi = lanes.begin() + to_index(start); vbi != lanes.begin() + to_index(end);
                 ++vbi)
                vbi->set(lane);

            // exit "find empty lane" loop
            break;
        }
    }
    return lane;
}
void
renderer::render_range(std::vector<entity>& entities, interval interval)
{
    assert(!entities.empty());

    const auto render_start = interval.start;
    const auto render_end = interval.end;
    assert(render_start <= render_end);

    auto selected_entities = select_from(entities);

    auto lanes_n = lanes_in_interval(interval);
    if (lanes_n == 0)
        return;

    style_info specs;
    specs.font = font;
    specs.font_size = font_size;
    specs.max_dimension = max_dim();
    specs.dimension = specs.max_dimension / lanes_n;

    const auto bin_start = to_index(render_start);
    const auto bin_end = to_index(render_end);
    const auto bin_len = utlz::limit<int>(0, spec::max_bins, bin_end - bin_start);
    specs.scale = get_scale(bin_len);

    std::fill(lanes.begin(), lanes.end(), std::bitset<128>());
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
        specs.lane_index = lane_index(lanes_n, entity_start, entity_end);
        specs.color.r = e.get().id * colour_incr;
        specs.color.g = specs.color.g + colour_incr;

        style->render(specs, e);
    }

    SDL_RenderPresent(graphics::get().ren);
}

////////////////////////////////////////////////////////////////////////////////
// stylist_v
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
    render_text_v(specs.font, text_color, rt, e.name.c_str(), specs.font_size);
}

////////////////////////////////////////////////////////////////////////////////
// vertical
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

////////////////////////////////////////////////////////////////////////////////
// horizontal
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

            render_text_h(font,
                          colors::federal_blue,
                          grid_label_bounds,
                          std::to_string(i - (int)zero_point).c_str(),
                          28);
        }
    }
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

////////////////////////////////////////////////////////////////////////////////
// stylist_h
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
    render_text_h(specs.font, text_color, offset, e.name.c_str(), specs.font_size);
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

////////////////////////////////////////////////////////////////////////////////
// stylist_v_line
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
    auto line_color = palette::broken[e.id % 5];
    float center_x = r.x + r.w / 2;
    draw_line(point{ center_x, r.y }, point{ center_x, r.y + r.h }, line_color);

    color dot_color = line_color;
    float dot_radius = 3.0f;

    draw_filled_circle(center_x, r.y, dot_radius, dot_color);
    draw_filled_circle(center_x, r.y + r.h, dot_radius, dot_color);

    // indicator line connecting text and lane box
    auto indicator_color = line_color;
    indicator_color.a = 128;
    draw_line(point{ r.x + r.w / 2 + 1.0f, r.y + specs.font_size / 2 },
              point{ (float)specs.max_dimension + 20, r.y + specs.font_size / 2 },
              indicator_color);

    auto rt = text_bounds(specs);
    render_text_v(specs.font, colors::night, rt, e.name.c_str(), specs.font_size);
}

void
stylist_v_line::draw_lane_dots(style_info specs, rect bounds)
{
}

////////////////////////////////////////////////////////////////////////////////
// stylist_h_line
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
    auto line_color = palette::broken[e.id % 5];
    draw_line(point{ r.x, center_y }, point{ r.x + r.w, center_y }, line_color);

    // Dots for caps
    auto& dot_color = line_color;
    float dot_radius = 3.0f;
    draw_filled_circle(r.x, center_y, dot_radius, dot_color);
    draw_filled_circle(r.x + r.w, center_y, dot_radius, dot_color);

    // Text
    auto offset = rect{ r.x, r.y - specs.font_size / 2, std::max(r.w, 100.0f), r.h };
    render_text_h(specs.font, colors::night, offset, e.name.c_str(), specs.font_size);
}

} // namespace tls