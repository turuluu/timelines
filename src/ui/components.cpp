#include "base.hpp"
#include "components.hpp"
#include "../rendering.hpp"
namespace tls
{
namespace ui
{
time_indicator::time_indicator() {}

void
time_indicator::draw(const context& ctx)
{
    if (!ctx.gui)
        return;

    float w = 100;
    float h = 50;
    rect r{ coord - (w * 0.5f), bounds.y + bounds.h - h, w, h };
    point start{ coord, bounds.y + bounds.h };
    point end{ coord, 0 };
    draw_line(start, end, colors::great_grey, 1);
    draw_text(ctx.gui->get_renderer().font, colors::broken_blue, r, "oh hi");
}
} // namespace ui
} // namespace tls