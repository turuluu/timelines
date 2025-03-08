#pragma once

#include "core.hpp"
#include "sdl/graphics.hpp"
#include "utilities.hpp"

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

    size_t entities_in_interval(Interval interval) const;

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

    void set_controller(RenderingController* controller);

    virtual void render_range(std::vector<Entity>& _entities, Interval* interval) = 0;

    virtual void draw_grid(Interval interval, const double scale_x) const {};

    // TODO : consider other options, state in interface..
    std::unique_ptr<EntityStyle> style;
    RenderingController* controller;
    Interval rendering_interval;
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
        SDL_SetRenderDrawColor(Graphics::get().ren, 0xFF, 0xFF, 0xFF, 0x20);
        SDL_RenderDrawRect(Graphics::get().ren, &r);

        // fill
        SDL_SetRenderDrawColor(Graphics::get().ren,
                               0x9F - (colour_fill * 0.5f),
                               0x90 + (0xFF - colour_fill) * 0.2f,
                               0xFF - (colour_fill * 0.8f),
                               0x70);
        SDL_RenderFillRect(Graphics::get().ren, &r);
    }
};

struct HBoxStyle : EntityStyle
{
    void render(SDL_Rect r, const Entity& e, u8 colour_border, u8 colour_fill) const override;

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
        Interval* interval = &renderer->rendering_interval;
        Interval timescaled = new_scaled_interval(delta_y, interval, x);

        // TODO : shift range when zooming in to keep center of zoom under the
        // mouse pointer
        const int mid_x = screen_dim / 2;
        constexpr double scale = 0.5e-2f;
        int rel_mid_point = (x - mid_x) * (delta_y * scale);
        *interval = timescaled;
    }

    void render() { renderer->render_range(core.data, &renderer->rendering_interval); }

    void button_left_drag(MouseMove m, const float multiplier = 1.5f) const
    {
        if (renderer == nullptr)
            return;

        const i32 multiplied_value =
          is_horizontal() ? ((float)-m.x) * multiplier : ((float)-m.y) * multiplier;

        Interval* year_range = &renderer->rendering_interval;
        Interval adjusted = new_relative_interval(multiplied_value, year_range);
        *year_range = adjusted;
    }

    void toggle_renderer()
    {
        toggle = !toggle;
        renderer = renderer_container[(int)toggle].get();
        renderer->render_range(core.data, &renderer_container[!toggle]->rendering_interval);
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
        SDL_SetRenderDrawColor(Graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(Graphics::get().ren);

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
    void render_range(std::vector<Entity>& _entities, Interval* interval_ptr) override;


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

        SDL_SetRenderDrawColor(Graphics::get().ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(Graphics::get().ren);

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

    void draw_grid(Interval interval, const double scale_x) const override;

    void render_range(Core::Entities& _entities, Interval* year_range) override;

    void test()
    {
        SDL_Rect r;
        r.x = 10;
        r.y = 10;
        r.w = 100;
        r.h = 100;

        // outline
        SDL_SetRenderDrawColor(Graphics::get().ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawRect(Graphics::get().ren, &r);

        // fill
        SDL_RenderFillRect(Graphics::get().ren, &r);

        SDL_Color color{ 255, 255, 255 };
        SDL_RenderPresent(Graphics::get().ren);
    }
    size_t font_size;
    TTF_Font* font;
};

}