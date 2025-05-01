#include "base.hpp"
#include "../sdl/graphics.hpp"
namespace tls
{
namespace ui
{
struct text : component
{
    text() = default;

    std::string_view name() const override { return "text"; }
    void draw(const context& ctx) override;

    void refresh(const context& ctx) override;
    void layout(const context& ctx) override { bounds = ctx.bounds; }
    void set_text(std::string new_str) { str = new_str; }
    void set_font(font* new_font) { font = new_font; }

    font* font{ nullptr };
    color color{ colors::broken_blue };
    rect bounds;
    point coord{};
    std::string str;
};
struct time_indicator : component
{
    time_indicator() = default;

    std::string_view name() const override { return "time_indicator"; }
    void draw(const context& ctx) override;

    void refresh(const context& ctx) override;
    void layout(const context& ctx) override { bounds = ctx.bounds; }

    text text;
    color color{ colors::broken_blue };
    rect bounds;
    point coord{};
};

} // namespace ui
} // namespace tls