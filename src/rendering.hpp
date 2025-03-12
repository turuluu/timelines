#pragma once

#include "core.hpp"
#include "sdl/graphics.hpp"
#include "utilities.hpp"
#include <list>

namespace tls
{
struct timer_ifc
{
    virtual ~timer_ifc() = default;
    virtual void wait_ms(size_t delay_ms) const = 0;

    /**
     * time delta since initiation
     */
    [[nodiscard]] virtual size_t get_ms_since_start() const = 0;
};

struct mouse_move
{
    i32 x;
    i32 y;
};

using rect = SDL_Rect;
struct colour
{
    u8 border = 255;
    u8 fill = 255;
};
struct style_info
{
    colour colour;
    int d;
    int max_d;
    int rect_start;
    int rect_end;
    double scale;
    size_t lane_index;

    size_t font_size;
    TTF_Font* font;
};

struct rendering_controller;
struct fashion_statement;
struct feng_shui
{
    using entity_ptr = std::unique_ptr<entity>;

    feng_shui()
      : id(gen_id())
      , font_size(36)
      , font{ get_title_font(font_size) }
    {
    }

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

    size_t entities_in_interval(interval interval) const;

    size_t lane(size_t max_entities_in_interval, int start, int end)
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

    virtual ~feng_shui()
    {
        if (font != nullptr)
            TTF_CloseFont(font);
    }

    void set_controller(rendering_controller* controller);

    virtual int max_dim() = 0;

    virtual double get_scale(double bin_len) = 0;

    virtual void draw_grid(interval interval, const double scale_x) const {};

    virtual void render_range(std::vector<entity>& _entities, interval interval);

    friend bool operator==(const feng_shui& lhs, const feng_shui& rhs) { return lhs.id == rhs.id; }

    friend bool operator!=(const feng_shui& lhs, const feng_shui& rhs) { return !(lhs == rhs); }

    // TODO : consider other options, state in interface..
    const int id;
    std::unique_ptr<fashion_statement> style;
    rendering_controller* controller;
    interval rendering_interval;
    std::vector<u8> lanes;
    orientation orientation = orientation::horizontal;

    // TODO : refactor to style
    size_t font_size;
    TTF_Font* font;
};

struct fashion_statement
{
    virtual ~fashion_statement() = default;
    virtual rect lane_bounds(style_info bi) = 0;
    virtual rect text_bounds(style_info bi) = 0;
    virtual void render(style_info specs, const entity& e) = 0;
};

struct stylist_v : fashion_statement
{
    rect lane_bounds(style_info bi) override;
    rect text_bounds(style_info bi) override;
    void render(style_info specs, const entity& e) override;
};

struct stylist_h : fashion_statement
{
    rect lane_bounds(style_info bi) override;
    rect text_bounds(style_info bi) override;
    void render(style_info specs, const entity& e) override;
};

struct rendering_controller
{
    explicit rendering_controller(core& core)
      : core(core)
    {
    }

    bool is_horizontal() const { return get_renderer().orientation == feng_shui::orientation::horizontal; }
    bool is_vertical() const { return get_renderer().orientation == feng_shui::orientation::vertical; }
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

        // TODO : shift range when zooming in to keep center of zoom under the
        // mouse pointer
        const int mid_x = screen_dim / 2;
        constexpr double scale = 0.5e-2f;
        int rel_mid_point = (x - mid_x) * (delta_y * scale);
        interval = timescaled;
    }

    void render() { get_renderer().render_range(core.data, get_renderer().rendering_interval); }

    void button_left_drag(mouse_move m, const float multiplier = 1.5f)
    {
        if (!is_renderer_set())
            return;

        const i32 multiplied_value =
          is_horizontal() ? ((float)-m.x) * multiplier : ((float)-m.y) * multiplier;

        auto& interval = get_renderer().rendering_interval;
        struct interval adjusted = new_relative_interval(multiplied_value, interval);
        interval = adjusted;
    }

    bool is_renderer_set() const { return renderer_idx >= 0; }

    void toggle_renderer()
    {
        toggle = !toggle;
        renderer_idx = (int)toggle;
        get_renderer().render_range(core.data, renderer_container[!toggle]->rendering_interval);
    }

    void button_right_drag() {}

    void set_current(feng_shui& renderer_ref)
    {
        int i = 0;
        while (i < renderer_container.size() && *renderer_container[i] != renderer_ref)
            ++i;

        assert(i < renderer_container.size());
        renderer_idx = i;
    }

    const feng_shui& get_renderer() const
    {
        assert(is_renderer_set());
        return *renderer_container[renderer_idx];
    }
    feng_shui& get_renderer()
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
    std::vector<std::unique_ptr<feng_shui>> renderer_container;
    int renderer_idx = -1;
};

struct vertical : feng_shui
{
    vertical()
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        orientation = orientation::vertical;

        assert(font != nullptr);

        // TODO : clean up to their own wrapper
        SDL_SetRenderDrawColor(graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(graphics::get().ren);

        lanes.resize(spec::max_bins);

        style = std::make_unique<stylist_v>();
    }

    int max_dim() override;
    double get_scale(double bin_len) override;
};

struct horizontal : feng_shui
{
    horizontal()
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";

        assert(font != nullptr);

        SDL_SetRenderDrawColor(graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(graphics::get().ren);

        lanes.resize(spec::max_bins);

        style = std::make_unique<stylist_h>();
    }

    void draw_grid(interval interval, double scale_x) const override;
    int max_dim() override;
    double get_scale(double bin_len) override;
};

}