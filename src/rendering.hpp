#pragma once

#include "core.hpp"
#include "details/graphics.hpp"
#include "details/utilities.hpp"

namespace tls
{
struct ITimer
{
    virtual ~ITimer() = default;
    virtual void wait_ms(size_t delay_ms) const = 0;

    /**
     * time delta since initiation
     */
    [[nodiscard]] virtual size_t get_ms_since_start() const = 0;
};

struct MouseMove
{
    i32 x;
    i32 y;
};

struct RenderingController;
struct EntityStyle;
struct Renderer
{
    using EntityPtr = std::unique_ptr<Entity>;

    enum class Flavour : int
    {
        Horizontal = 1,
        Vertical = 2
    };

    Core::Entities select_from(const Core::Entities& entities) const;

    size_t entities_in_interval(int start, int end) const;

    size_t lane(size_t max_entities_in_interval, int start, int end)
    {
        size_t lane = 0;
        for (lane = 0; lane < max_entities_in_interval; ++lane)
        {
            auto lanes_begin = std::begin(lanes) + year_to_index(start);
            auto lanes_end = std::begin(lanes) + year_to_index(end);
            if (std::all_of(lanes_begin,
                            lanes_end,
                            [&](u8 ui) { return (bool)u8(1 << lane & ui); }))
            {
                // mark lane
                auto marked_begin = std::begin(lanes) + year_to_index(start);
                auto marked_end = std::begin(lanes) + year_to_index(end);
                for (auto vbi = marked_begin; vbi != marked_end; ++vbi)
                    *vbi = (*vbi) - (1 << lane);
                break;
            }
        }
        return lane;
    }

    virtual ~Renderer() = default;

    void set_controller(RenderingController* controller);

    virtual void render_range(std::vector<Entity>& _entities, YearRange* yrRange) = 0;

    virtual void draw_grid(int year_start, int year_end, const double scale_x) const {};

    // TODO : consider other options, state in interface..
    std::unique_ptr<EntityStyle> style;
    RenderingController* controller;
    YearRange year_range;
    std::vector<u8> lanes;
    Flavour flavour = Flavour::Horizontal;
};

struct ThreadPool
{
};

struct EntityStyle
{
    virtual ~EntityStyle() = default;
    virtual void render(SDL_Rect r, const Entity& e, u8 colour_border, u8 colour_fill) const = 0;

    size_t font_size;
    TTF_Font* font;
};

struct VBoxStyle : EntityStyle
{
    void render(SDL_Rect r, const Entity& e, u8 colour_border, u8 colour_fill) const override
    {
        // outline
        SDL_SetRenderDrawColor(g.ren, 0xFF, 0xFF, 0xFF, 0x20);
        SDL_RenderDrawRect(g.ren, &r);

        // fill
        SDL_SetRenderDrawColor(g.ren,
                               0x9F - (colour_fill * 0.5f),
                               0x90 + (0xFF - colour_fill) * 0.2f,
                               0xFF - (colour_fill * 0.8f),
                               0x70);
        SDL_RenderFillRect(g.ren, &r);
    }
};

struct HBoxStyle : EntityStyle
{
    void render(SDL_Rect r, const Entity& e, u8 colour_border, u8 colour_fill) const override
{
        // fill
        SDL_SetRenderDrawColor(g.ren,
                               0x9F - (colour_fill * 0.5f),
                               0x90 + (0xFF - colour_fill) * 0.2f,
                               0xFF - (colour_fill * 0.8f),
                               0x70);
        SDL_RenderFillRect(g.ren, &r);

        // outline
        SDL_SetRenderDrawColor(g.ren, 0xFF, 0xFF, 0xFF, 0x20);

        SDL_RenderDrawRect(g.ren, &r);
        SDL_Color color{ 255, 255, 255, 0x80 };
        render_text_2(font, &color, &r, e.name.c_str(), font_size);
}
};

struct RenderingController
{
    explicit RenderingController(Core& core)
      : core(core)
    {
    }

    bool is_horizontal() const { return renderer->flavour == Renderer::Flavour::Horizontal; }
    bool is_vertical() const { return renderer->flavour == Renderer::Flavour::Vertical; }
    void set_refresh_rate(size_t refresh_rate) { frame_interval_ms = 1000 / refresh_rate; }
    void wait_until_next_frame() const
    {
        auto time_delta_ms = timer->get_ms_since_start();
        auto elapsed_ms = time_delta_ms - last_frame_ms;
        if (elapsed_ms >= frame_interval_ms)
            return;
        else
            timer->wait_ms(frame_interval_ms - elapsed_ms);
    }

    void scroll_y(int delta_y, int x, int y) const
    {
        if (renderer == nullptr)
            return;

        auto screen_dim = is_horizontal() ? spec::screen_w : spec::screen_h;
        YearRange* year_range = &renderer->year_range;
        YearRange timescaled = new_scaled_year_range(delta_y, year_range, x);

        // TODO : shift range when zooming in to keep center of zoom under the
        // mouse pointer
        const int mid_x = screen_dim / 2;
        constexpr double scale = 0.5e-2f;
        int rel_mid_point = (x - mid_x) * (delta_y * scale);
        *year_range = timescaled;
    }

    void render() { renderer->render_range(core.data, &renderer->year_range); }

    void button_left_drag(MouseMove m, const float multiplier = 1.5f) const
    {
        if (renderer == nullptr)
            return;

        const i32 multiplied_value =
          is_horizontal() ? ((float)-m.x) * multiplier : ((float)-m.y) * multiplier;

        YearRange* year_range = &renderer->year_range;
        YearRange adjusted = new_relative_year_range(multiplied_value, year_range);
        *year_range = adjusted;
    }

    void toggle_renderer()
    {
        toggle = !toggle;
        renderer = renderer_container[(int)toggle].get();
        renderer->render_range(core.data, &renderer_container[!toggle]->year_range);
    }

    void button_right_drag() {}

    void set_current(Renderer* renderer_ptr)
    {
        auto it =
          std::find_if(renderer_container.begin(),
                       renderer_container.end(),
                       [&renderer_ptr](auto& uniq_ptr) { return uniq_ptr.get() == renderer_ptr; });

        if (it != renderer_container.end())
            renderer = it->get();
        // TODO : error handling
    }

    Renderer* own(Renderer* new_renderer)
    {
        new_renderer->set_controller(this);
        renderer_container.emplace_back(new_renderer);
        return renderer_container.back().get();
    }

    Core& core;
    bool toggle{ true };
    size_t frame_interval_ms{};
    size_t last_frame_ms{};
    std::unique_ptr<ITimer> timer;
    std::vector<std::unique_ptr<Renderer>> renderer_container;
    Renderer* renderer = nullptr;
};

struct Vertical : Renderer
{
    Vertical()
      : font_size(36)
      , font{ get_title_font(font_size) }
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        flavour = Renderer::Flavour::Vertical;

        assert(font != nullptr);

        // TODO : clean up to their own wrapper
        SDL_SetRenderDrawColor(g.ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(g.ren);

        lanes.resize(spec::max_bins);

        style = std::make_unique<VBoxStyle>();
        style->font = font;
        style->font_size = font_size;
    }

    ~Vertical() override
    {
        if (font != nullptr)
            TTF_CloseFont(font);
    }

    // TODO : feels quite repeated too..
    void render_range(std::vector<Entity>& _entities, YearRange* yr_range) override
    {
        assert(!_entities.empty());

        const auto render_start = yr_range->start;
        const auto render_end = yr_range->end;
        assert(render_start <= render_end);

        auto maxW = spec::screen_w / 2;

        Core::Entities selected_entities = select_from(_entities);

        auto max_entities_in_interval = entities_in_interval(render_start, render_end);
        if (max_entities_in_interval == 0)
            return;

        auto w = maxW / max_entities_in_interval;

        clear();

        u8 colour_incr = 255 / _entities.size();

        const auto render_start_y = year_to_index(render_start);
        const auto render_end_y = year_to_index(render_end);
        const auto interval = util::limit<int>(0, spec::max_bins, render_end_y - render_start_y);
        const double scale_y = spec::screen_h / (double)interval;

        std::fill(lanes.begin(), lanes.end(), std::numeric_limits<uint8_t>::max());

        for (auto& e : selected_entities)
        {
            //            std::cout << "Entity found, rendering..\n";
            // draw_grid(render_start, render_end, xScale);

            const int entity_start_year = e.start_year;
            const int entity_end_year = e.end_year;

            auto start_bound = std::max(entity_start_year, render_start);
            const int rect_start_y = year_to_index(start_bound) - render_start_y;

            auto end_bound = std::min(entity_end_year, render_end);
            const int rect_end_y = year_to_index(end_bound) - render_start_y;

            // non const part
            const size_t lane_index =
              lane(max_entities_in_interval, entity_start_year, entity_end_year);

            //            e->bounds.x = 10 + (w * lane_index);
            //            e->bounds.y = rect_start_y * scale_y;
            //            e->bounds.h = (rect_end_y - rect_start_y) * scale_y;
            //            e->bounds.w = w;

            //            r.x = e->bounds.x;
            //            r.y = e->bounds.y;
            //            r.w = e->bounds.w;
            //            r.h = e->bounds.h;

            SDL_Rect r;
            r.x = 10 + (w * lane_index);
            r.y = rect_start_y * scale_y;
            r.h = (rect_end_y - rect_start_y) * scale_y;
            r.w = w;
            const u8 colour_fill = e.id * colour_incr;
            const u8 colour_border = colour_fill + colour_incr;

            style->render(r, e, colour_border, colour_fill);
            SDL_RenderDrawLine(g.ren,
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
            render_text(font, &color, &rt, e.name.c_str(), font_size);
        }

        SDL_RenderPresent(g.ren);
        // TODO : probably due to scope
        //        SDL_Delay(50);
    }

    size_t font_size;
    TTF_Font* font;
};

struct Horizontal : Renderer
{
    Horizontal()
      : font_size(36)
      , font{ get_title_font(font_size) }
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";

        assert(font != nullptr);

        SDL_SetRenderDrawColor(g.ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(g.ren);

        lanes.resize(spec::max_bins);

        style = std::make_unique<HBoxStyle>();
    }

    ~Horizontal() override
    {
        if (font != nullptr)
            TTF_CloseFont(font);
    }

    void draw_grid(int year_start, int year_end, const double scale_x) const override
    {
        const int interval = year_end - year_start;
        int splits = interval / 8.0f;
        splits = ((int)(splits / 10.0f)) * 10;
        splits = splits > 0 ? splits : 1;
        auto start_idx = year_to_index(year_range.start);
        for (int i = year_start; i < year_end; ++i)
        {
            if (i % splits == 0)
            {
                SDL_SetRenderDrawColor(g.ren, 0x5F, 0x5F, 0x5F, 0x20);
                const int x = (year_to_index(i) - start_idx) * scale_x;
                SDL_RenderDrawLine(g.ren, x, 0, x, spec::screen_h);

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

    void render_range(Core::Entities& _entities, YearRange* year_range) override
    {
        const auto render_start = year_range->start;
        const auto render_end = year_range->end;

        assert(render_start <= render_end);
        auto max_h = spec::screen_h - 80;

        Core::Entities selected_entities = select_from(_entities);

        auto max_entities_in_interval = entities_in_interval(render_start, render_end);
        if (max_entities_in_interval == 0)
            return;

        auto h = max_h / max_entities_in_interval;

        clear();

        assert(_entities.size() > 0);
        u8 colour_incr = 255 / _entities.size();

        const auto render_start_x = year_to_index(render_start);
        const auto render_end_x = year_to_index(render_end);
        const auto interval = util::limit<int>(0, spec::max_bins, render_end_x - render_start_x);
        const double xScale = spec::screen_w / (double)interval;

        std::fill(lanes.begin(), lanes.end(), std::numeric_limits<uint8_t>::max());

        for (auto& e : selected_entities)
        {
            draw_grid(render_start, render_end, xScale);

            const int entity_start = e.start_year;
            const int entity_end = e.end_year;

            auto bound_start = std::max(entity_start, render_start);
            const int rect_start_x = year_to_index(bound_start) - render_start_x;

            auto bound_end = std::min(entity_end, render_end);
            const int rect_end_x = year_to_index(bound_end) - render_start_x;

            // non const part
            const size_t lane_idx = lane(max_entities_in_interval, entity_start, entity_end);

            SDL_Rect r;
            r.x = rect_start_x * xScale;
            r.y = 10 + (h * lane_idx);
            r.w = (rect_end_x - rect_start_x) * xScale;
            r.h = h;

            const u8 fill_colour = e.id * colour_incr;
            const u8 border_colour = fill_colour + colour_incr;

            style->render(r, e, border_colour, fill_colour);
        }

        SDL_RenderPresent(g.ren);
        // TODO : probably due to scope
        //        SDL_Delay(50);
    }

    void test()
    {
        SDL_Rect r;
        r.x = 10;
        r.y = 10;
        r.w = 100;
        r.h = 100;

        // outline
        SDL_SetRenderDrawColor(g.ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawRect(g.ren, &r);

        // fill
        SDL_RenderFillRect(g.ren, &r);

        SDL_Color color{ 255, 255, 255 };
        SDL_RenderPresent(g.ren);
    }
    size_t font_size;
    TTF_Font* font;
};

// render_range(..., auto max_h = screen_h - 80, screen_h)
// static void
// render_range(Renderer& renderer,
//              Core::Entities& _entities,
//              YearRange* year_range,
//              int max_dim,
//              int scale_nom)
// {
//     const auto render_start = year_range->start;
//     const auto render_end = year_range->end;
//
//     assert(render_start <= render_end);
//
//     Core::Entities selected_entities = renderer.select_from(_entities);
//
//     auto max_entities_in_interval = renderer.entities_in_interval(render_start, render_end);
//     if (max_entities_in_interval == 0)
//         return;
//
//     auto h = max_dim / max_entities_in_interval;
//
//     clear();
//
//     assert(_entities.size() > 0);
//     u8 colour_incr = 255 / _entities.size();
//
//     const auto render_start_pos = year_to_index(render_start);
//     const auto render_end_pos = year_to_index(render_end);
//     const auto interval = util::limit<int>(0, spec::max_bins, render_end_pos - render_start_pos);
//     const double scale = scale_nom / (double)interval;
//
//     std::fill(renderer.lanes.begin(), renderer.lanes.end(), std::numeric_limits<uint8_t>::max());
//
//     for (auto& e : selected_entities)
//     {
//         renderer.draw_grid(render_start, render_end, scale);
//
//         const int entity_start = e.start_year;
//         const int entity_end = e.end_year;
//
//         auto bound_start = std::max(entity_start, render_start);
//         const int rect_start_x = year_to_index(bound_start) - render_start_pos;
//
//         auto bound_end = std::min(entity_end, render_end);
//         const int rect_end_x = year_to_index(bound_end) - render_start_pos;
//
//         // non const part
//         const size_t lane_idx = renderer.lane(max_entities_in_interval, entity_start, entity_end);
//
//         // TODO : method or lambda call
//         SDL_Rect r;
//         r.x = rect_start_x * scale;
//         r.y = 10 + (h * lane_idx);
//         r.w = (rect_end_x - rect_start_x) * scale;
//         r.h = h;
//
//         const u8 fillColour = e.id * colour_incr;
//         const u8 borderColour = fillColour + colour_incr;
//
//         renderer.render_entity_box(r, e, borderColour, fillColour);
//     }
//
//     SDL_RenderPresent(g.ren);
//     // TODO : probably due to scope
//     //        SDL_Delay(50);
// }
}