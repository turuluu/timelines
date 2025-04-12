#pragma once

#include <cassert>
#include <iosfwd>
#include <list>

#include "backend.hpp"
#include "core.hpp"
#include "types.hpp"

namespace tls
{

struct style_info
{
    color color;
    int dimension;
    int max_dimension;
    int rect_start;
    int rect_end;
    double scale;
    size_t lane_index;

    size_t font_size;
    font* font;
};

struct rendering_controller;
struct stylist_base;
struct renderer
{
    using entity_ptr = std::unique_ptr<entity>;

    renderer();

    static int gen_id()
    {
        static int id = 0;
        return id++;
    }

    enum class orientation : int
    {
        horizontal = 1,
        vertical = 2
    };

    std::vector<std::reference_wrapper<const entity>> select_from(
      const core::entities& entities) const;

    size_t lanes_in_interval(interval interval) const;

    size_t lane(size_t max_entities_in_interval, time_point start, time_point end)
    {
        size_t lane = 0;
        for (lane = 0; lane < max_entities_in_interval; ++lane)
        {
            auto lanes_begin = std::begin(lanes) + to_index(start);
            auto lanes_end = std::begin(lanes) + to_index(end);
            if (std::all_of(lanes_begin,
                            lanes_end,
                            [&](u8 ui) { return (bool)u8(1 << lane & ui); }))
            {
                // mark lane
                auto marked_begin = std::begin(lanes) + to_index(start);
                auto marked_end = std::begin(lanes) + to_index(end);
                for (auto vbi = marked_begin; vbi != marked_end; ++vbi)
                    *vbi = (*vbi) - (1 << lane);
                break;
            }
        }
        return lane;
    }

    virtual ~renderer();

    void set_controller(rendering_controller* controller);

    virtual int max_dim() = 0;

    virtual double get_scale(double bin_len) = 0;

    virtual void draw_grid(interval interval, const double scale_x) const {};

    virtual void render_range(std::vector<entity>& _entities, interval interval);

    friend bool operator==(const renderer& lhs, const renderer& rhs) { return lhs.id == rhs.id; }

    friend bool operator!=(const renderer& lhs, const renderer& rhs) { return !(lhs == rhs); }

    // TODO : consider other options, state in interface..
    const int id;
    std::unique_ptr<stylist_base> style;
    rendering_controller* controller;
    interval rendering_interval;
    std::vector<u8> lanes;
    orientation orientation = orientation::horizontal;

    // TODO : refactor to style
    size_t font_size;
    font* font;
};

struct stylist_base
{
    virtual ~stylist_base() = default;
    virtual rect lane_bounds(style_info bi) = 0;
    virtual rect text_bounds(style_info bi) = 0;
    virtual void render(style_info specs, const entity& e) = 0;
};

struct stylist_v : stylist_base
{
    rect lane_bounds(style_info s) override;
    rect text_bounds(style_info s) override;
    void render(style_info specs, const entity& e) override;
};

struct stylist_h : stylist_base
{
    rect lane_bounds(style_info s) override;
    rect text_bounds(style_info bi) override;
    void render(style_info specs, const entity& e) override;
};

struct stylist_v_line : stylist_base
{
    rect lane_bounds(style_info s) override;
    rect text_bounds(style_info s) override;
    void render(style_info specs, const entity& e) override;

    void draw_lane_line(style_info specs, rect bounds);
    void draw_lane_dots(style_info specs, rect bounds);
};

struct stylist_h_line : stylist_base
{
    rect lane_bounds(style_info bi) override;
    rect text_bounds(style_info bi) override;
    void render(style_info specs, const entity& e) override;

    void draw_lane_line(style_info specs, rect bounds);
    void draw_lane_dots(style_info specs, rect bounds);
};

struct rendering_controller
{
    explicit rendering_controller(core& core)
      : core(core)
    {
    }

    bool is_horizontal() const
    {
        return get_renderer().orientation == renderer::orientation::horizontal;
    }
    bool is_vertical() const
    {
        return get_renderer().orientation == renderer::orientation::vertical;
    }
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

    void scroll_y(int delta_y, int x, int y)
    {
        if (renderer_idx < 0)
            return;

        auto screen_dim = is_horizontal() ? spec::screen_w : spec::screen_h;
        auto& interval = get_renderer().rendering_interval;
        struct interval timescaled = new_scaled_interval(delta_y, interval, x);

        /*
        // TODO : shift range when zooming in to keep center of zoom under the
        // mouse pointer
        const int mid_x = screen_dim / 2;
        constexpr double scale = 0.5e-2f;
        int rel_mid_point = (x - mid_x) * (delta_y * scale);
        */
        interval = timescaled;
    }

    void render() { get_renderer().render_range(core.data, get_renderer().rendering_interval); }

    void button_left_drag(mouse_move m, const float multiplier = 1.5f);

    bool is_renderer_set() const { return renderer_idx >= 0; }

    void toggle_renderer()
    {
        toggle = !toggle;
        renderer_idx = (int)toggle;
        get_renderer().render_range(core.data, renderer_container[!toggle]->rendering_interval);
    }

    void button_right_drag() {}

    void set_current(renderer& renderer_ref)
    {
        int i = 0;
        while (i < renderer_container.size() && *renderer_container[i] != renderer_ref)
            ++i;

        assert(i < renderer_container.size());
        renderer_idx = i;
    }

    const renderer& get_renderer() const
    {
        assert(is_renderer_set());
        return *renderer_container[renderer_idx];
    }
    renderer& get_renderer()
    {
        assert(is_renderer_set());
        return *renderer_container[renderer_idx];
    }

    template<typename T>
    T& make()
    {
        auto& new_renderer = renderer_container.emplace_back(std::make_unique<T>());
        new_renderer->set_controller(this);
        return (T&)*new_renderer;
    }

    core& core;
    bool toggle{ true };
    size_t frame_interval_ms{};
    size_t last_frame_ms{};
    std::unique_ptr<timer_ifc> timer;
    std::vector<std::unique_ptr<renderer>> renderer_container;
    int renderer_idx = -1;
};

struct vertical : renderer
{
    vertical();

    int max_dim() override;
    double get_scale(double bin_len) override;
};

struct horizontal : renderer
{
    horizontal();

    void draw_grid(interval interval, double scale_x) const override;
    int max_dim() override;
    double get_scale(double bin_len) override;
};

} // namespace tls