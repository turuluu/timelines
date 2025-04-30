#include "components.hpp"
#include "../rendering.hpp"
#include "base.hpp"
namespace tls
{
namespace ui
{

void
time_indicator::draw(const context& ctx)
{
    if (!ctx.gui)
        return;

    auto& renderer = ctx.gui->get_renderer();
    bool is_horizontal = ctx.gui->is_horizontal();

    auto interval = renderer.rendering_interval;
    auto ratio = is_horizontal ? double(coord.x) / double(ctx.bounds.w)
                               : double(coord.y) / double(ctx.bounds.h);
    auto ind_pt = clock::interpolate(interval.start, interval.end, ratio);

    int rounded = ind_pt.time_since_epoch().count();
    int digits = std::log10(abs(rounded) + 10) + (rounded < 0 ? 1 : 0);
    auto text = std::to_string(rounded).c_str();

    float w = 10 * digits;
    float h = 25;
    rect r;
    if (is_horizontal)
        r = { coord.x - (w * 0.5f), 10, w, h };
    else
        r = { 10, coord.y - (h * 0.5f), w, h };

    draw_text(renderer.font, color, r, text);
}

void
text::draw(const context& ctx)
{

}
} // namespace ui
} // namespace tls