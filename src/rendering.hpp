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

struct rendering_controller;
struct entity_style;
struct Renderer
{
    using EntityPtr = std::unique_ptr<entity>;

    Renderer()
      : id(gen_id())
    {
    }
    static int gen_id()
    {
        static int id = 0;
        return id++;
    }

    enum class Flavour : int
    {
        Horizontal = 1,
        Vertical = 2
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

    virtual ~Renderer() = default;

    void set_controller(rendering_controller* controller);

    virtual void render_range(std::vector<entity>& _entities, interval interval) = 0;

    virtual void draw_grid(interval interval, const double scale_x) const {};

    friend bool operator==(const Renderer& lhs, const Renderer& rhs) { return lhs.id == rhs.id; }

    friend bool operator!=(const Renderer& lhs, const Renderer& rhs) { return !(lhs == rhs); }

    // TODO : consider other options, state in interface..
    const int id;
    std::unique_ptr<entity_style> style;
    rendering_controller* controller;
    interval rendering_interval;
    std::vector<u8> lanes;
    Flavour flavour = Flavour::Horizontal;
};

struct entity_style
{
    virtual ~entity_style() = default;
    virtual void render(SDL_Rect r, const entity& e, u8 colour_border, u8 colour_fill) const = 0;

    size_t font_size;
    TTF_Font* font;
};

struct VBoxStyle : entity_style
{
    void render(SDL_Rect r, const entity& e, u8 colour_border, u8 colour_fill) const override
    {
        // outline
        SDL_SetRenderDrawColor(graphics::get().ren, 0xFF, 0xFF, 0xFF, 0x20);
        SDL_RenderDrawRect(graphics::get().ren, &r);

        // fill
        SDL_SetRenderDrawColor(graphics::get().ren,
                               0x9F - (colour_fill * 0.5f),
                               0x90 + (0xFF - colour_fill) * 0.2f,
                               0xFF - (colour_fill * 0.8f),
                               0x70);
        SDL_RenderFillRect(graphics::get().ren, &r);
    }
};

struct HBoxStyle : entity_style
{
    void render(SDL_Rect r, const entity& e, u8 colour_border, u8 colour_fill) const override;
};

struct rendering_controller
{
    explicit rendering_controller(core& core)
      : core(core)
    {
    }

    bool is_horizontal() const { return get_renderer().flavour == Renderer::Flavour::Horizontal; }
    bool is_vertical() const { return get_renderer().flavour == Renderer::Flavour::Vertical; }
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

    void set_current(Renderer& renderer_ref)
    {
        int i = 0;
        while (i < renderer_container.size() && *renderer_container[i] != renderer_ref)
            ++i;

        assert(i < renderer_container.size());
        renderer_idx = i;
    }

    const Renderer& get_renderer() const
    {
        assert(is_renderer_set());
        return *renderer_container[renderer_idx];
    }
    Renderer& get_renderer()
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
    std::vector<std::unique_ptr<Renderer>> renderer_container;
    int renderer_idx = -1;
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
        SDL_SetRenderDrawColor(graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(graphics::get().ren);

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
    void render_range(std::vector<entity>& _entities, interval interval) override;

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

        SDL_SetRenderDrawColor(graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(graphics::get().ren);

        lanes.resize(spec::max_bins);

        style = std::make_unique<HBoxStyle>();
        style->font = font;
        style->font_size = font_size;
    }

    ~Horizontal() override
    {
        if (font != nullptr)
            TTF_CloseFont(font);
    }

    void draw_grid(interval interval, const double scale_x) const override;

    void render_range(core::entities& _entities, interval interval) override;

    void test()
    {
        SDL_Rect r;
        r.x = 10;
        r.y = 10;
        r.w = 100;
        r.h = 100;

        // outline
        SDL_SetRenderDrawColor(graphics::get().ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawRect(graphics::get().ren, &r);

        // fill
        SDL_RenderFillRect(graphics::get().ren, &r);

        SDL_Color color{ 255, 255, 255 };
        SDL_RenderPresent(graphics::get().ren);
    }
    size_t font_size;
    TTF_Font* font;
};

}