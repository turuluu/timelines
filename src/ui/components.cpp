#include "components.hpp"
#include "../rendering.hpp"
#include "base.hpp"
namespace tls
{
namespace ui
{

////////////////////////////////////////////////////////////////////////////////
// text
////////////////////////////////////////////////////////////////////////////////
void
text::refresh(const context& ctx)
{
    set_font(ctx.gui->get_renderer().font);
}

void
text::draw(const context& ctx)
{
    if (font)
        draw_text(font, color, bounds, str.c_str());
}


////////////////////////////////////////////////////////////////////////////////
// time_indicator
////////////////////////////////////////////////////////////////////////////////
void
time_indicator::refresh(const context& ctx)
{
    if (!ctx.gui)
        return;

    coord = { ctx.mouse.x, ctx.mouse.y };

    auto& renderer = ctx.gui->get_renderer();
    bool is_horizontal = ctx.gui->is_horizontal();

    auto interval = renderer.rendering_interval;
    auto ratio = is_horizontal ? double(coord.x) / double(ctx.bounds.w)
                               : double(coord.y) / double(ctx.bounds.h);
    auto ind_pt = clock::interpolate(interval.start, interval.end, ratio);

    int rounded = ind_pt.time_since_epoch().count();
    int digits = std::log10(abs(rounded) + 10) + (rounded < 0 ? 1 : 0);

    float w = 10 * digits;
    float h = 25;
    rect r;
    if (is_horizontal)
        r = { coord.x - (w * 0.5f), 10, w, h };
    else
        r = { 10, coord.y - (h * 0.5f), w, h };
    text.layout({ .bounds = r });
    text.set_text(std::to_string(rounded));
    text.refresh(ctx);
}
void
time_indicator::draw(const context& ctx)
{
    text.draw(ctx);
}

} // namespace ui
} // namespace tls