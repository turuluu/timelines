#include "graphics.hpp"
#include <utlz.hpp>
#include <filesystem>

#if USE_SDL
#include <SDL3_ttf/SDL_ttf.h>
#endif

namespace tls
{

#if USE_SDL
inline
#endif
  namespace sdl
{

void clear()
{
    // TODO : paint background
    u8 grey = 0x30;

    u8 r, gr, b, a;
    SDL_GetRenderDrawColor(graphics::get().ren, &r, &gr, &b, &a);
    auto c = colors::cardboard_white;
    SDL_SetRenderDrawColor(graphics::get().ren, c.r, c.g, c.b, c.a);
    SDL_RenderClear(graphics::get().ren);
    SDL_SetRenderDrawColor(graphics::get().ren, r, gr, b, a);
}

font*
get_title_font(int font_size)
{
    auto font_path = std::string(CMAKE_ROOT);
    font_path.append("/fonts/vera-fonts/dejavuSansMono.ttf");
    assert(std::filesystem::exists(font_path));

    auto font = TTF_OpenFont(font_path.c_str(), font_size);
    assert(font);
    return font;
}

void
destroy_font(TTF_Font* font)
{
    if (font != nullptr)
        TTF_CloseFont(font);
}

SDL_Color adapt(const color tls_color)
{
    return {tls_color.r, tls_color.g, tls_color.b, tls_color.a};
}
void draw_text(font* font, const color color, rect bounds, const char* text)
{
    if (font == nullptr || text == nullptr)
    {
        std::cout << "font was null. Exiting text rendering..";
        return;
    }

    SDL_Color sdl_color = adapt(color);
    SDL_Surface* surface;
    if (!(surface = TTF_RenderText_Blended(font, text, strlen(text), sdl_color)))
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << SDL_GetError() << "\n";
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(graphics::get().ren, surface);
    SDL_RenderTexture(graphics::get().ren, texture, NULL, &bounds);

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void render_text_v(font* font, const color color, rect text_bounds, const char* text, int ptsize)
{
    const int pt_size_smooth = ptsize / 2;
    const int cstrL = utlz::length(text);
    const int cstrW = cstrL * (pt_size_smooth / 2);

    SDL_FRect text_box;
    int margins = -1 * pt_size_smooth;
    int offset_msg_bounds_w = text_bounds.w + (2 * margins);
    text_box.w = std::min(cstrW, offset_msg_bounds_w);
    text_box.h = pt_size_smooth;
    text_box.x = text_bounds.x - margins;
    text_box.y = text_bounds.y + std::max((float)text_bounds.h - text_box.h, 0.0f) / 2;

    draw_text(font, color, text_box, text);
}

void render_text_h(font* font, const color color, rect text_bounds, const char* text, int ptsize)
{

    const int pt_size_smooth = ptsize / 2;
    const int cstr_l = utlz::length(text);
    const int cstr_w = cstr_l * (pt_size_smooth / 2);

    int margins = 2 * pt_size_smooth;
    int offset_bounds_w = text_bounds.w + (2 * margins);
    SDL_FRect text_box;
    text_box.x = text_bounds.x - margins + std::max(offset_bounds_w - cstr_w, 0) / 2;
    text_box.y = text_bounds.y + std::max((float)text_bounds.h - text_box.h, 0.0f) / 2;
    text_box.w = std::min(cstr_w, offset_bounds_w);
    text_box.h = pt_size_smooth;

    draw_text(font, color, text_box, text);
}

void draw_filled_rect(const rect rect, const color color) {
    SDL_SetRenderDrawColor(graphics::get().ren, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(graphics::get().ren, &rect);
}

void draw_rect(const rect rect, const color color) {
    SDL_SetRenderDrawColor(graphics::get().ren, color.r, color.g, color.b, color.a);
    SDL_RenderRect(graphics::get().ren, &rect);
}

void draw_line(point p0, point p1, const color color, float width) {
    SDL_SetRenderDrawColor(graphics::get().ren, color.r, color.g, color.b, color.a);
    
    // Perpendicular vector
    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    float length = std::sqrt(dx * dx + dy * dy);
    
    if (length == 0) return;
    
    // Normalize
    float nx = -dy / length;
    float ny = dx / length;
    
    float half_width = (width - 1) / 2.0f;
    
    // Center line
    SDL_RenderLine(graphics::get().ren, p0.x, p0.y, p1.x, p1.y);
    
    // Draw parallel lines on both sides
    for (float offset = 0.5f; offset <= half_width; offset += 1.0f) {
        point offset_p0 = { p0.x + nx * offset, p0.y + ny * offset };
        point offset_p1 = { p1.x + nx * offset, p1.y + ny * offset };
        SDL_RenderLine(graphics::get().ren, offset_p0.x, offset_p0.y, offset_p1.x, offset_p1.y);
        
        offset_p0 = { p0.x - nx * offset, p0.y - ny * offset };
        offset_p1 = { p1.x - nx * offset, p1.y - ny * offset };
        SDL_RenderLine(graphics::get().ren, offset_p0.x, offset_p0.y, offset_p1.x, offset_p1.y);
    }
}

void draw_filled_circle(float x, float y, float radius, const color color) {
    SDL_SetRenderDrawColor(graphics::get().ren, color.r, color.g, color.b, color.a);
    
    // Using the midpoint circle algorithm with fill
    float offset_x = 0;
    float offset_y = radius;
    float d = radius - 1;
    
    while (offset_y >= offset_x) {
        // fill
        SDL_RenderLine(graphics::get().ren, x - offset_y, y + offset_x, x + offset_y, y + offset_x);
        SDL_RenderLine(graphics::get().ren, x - offset_y, y - offset_x, x + offset_y, y - offset_x);
        SDL_RenderLine(graphics::get().ren, x - offset_x, y + offset_y, x + offset_x, y + offset_y);
        SDL_RenderLine(graphics::get().ren, x - offset_x, y - offset_y, x + offset_x, y - offset_y);
        
        if (d >= 2 * offset_x) {
            d -= 2 * offset_x + 1;
            offset_x += 1;
        }
        else if (d < 2 * (radius - offset_y)) {
            d += 2 * offset_y - 1;
            offset_y -= 1;
        }
        else {
            d += 2 * (offset_y - offset_x - 1);
            offset_y -= 1;
            offset_x += 1;
        }
    }
}

void draw_circle(float x, float y, float radius, const color color) {
    SDL_SetRenderDrawColor(graphics::get().ren, color.r, color.g, color.b, color.a);
    
    // Using the midpoint circle algorithm
    float offset_x = 0;
    float offset_y = radius;
    float d = radius - 1;
    
    while (offset_y >= offset_x) {
        SDL_RenderPoint(graphics::get().ren, x + offset_x, y + offset_y);
        SDL_RenderPoint(graphics::get().ren, x + offset_x, y - offset_y);
        SDL_RenderPoint(graphics::get().ren, x - offset_x, y + offset_y);
        SDL_RenderPoint(graphics::get().ren, x - offset_x, y - offset_y);
        SDL_RenderPoint(graphics::get().ren, x + offset_y, y + offset_x);
        SDL_RenderPoint(graphics::get().ren, x + offset_y, y - offset_x);
        SDL_RenderPoint(graphics::get().ren, x - offset_y, y + offset_x);
        SDL_RenderPoint(graphics::get().ren, x - offset_y, y - offset_x);
        
        if (d >= 2 * offset_x) {
            d -= 2 * offset_x + 1;
            offset_x += 1;
        }
        else if (d < 2 * (radius - offset_y)) {
            d += 2 * offset_y - 1;
            offset_y -= 1;
        }
        else {
            d += 2 * (offset_y - offset_x - 1);
            offset_y -= 1;
            offset_x += 1;
        }
    }
}

void draw_filled_triangle(point p0, point p1, point p2, color color) {
    SDL_SetRenderDrawColor(graphics::get().ren, color.r, color.g, color.b, color.a);
    
    // Sort vertices by y-coordinate
    if (p0.y > p1.y) { std::swap(p0, p1); }
    if (p0.y > p2.y) { std::swap(p0, p2); }
    if (p1.y > p2.y) { std::swap(p1, p2); }
    
    // Calculate slopes
    float dx12 = p1.x - p0.x;
    float dx13 = p2.x - p0.x;
    float dx23 = p2.x - p1.x;
    float dy12 = p1.y - p0.y;
    float dy13 = p2.y - p0.y;
    float dy23 = p2.y - p1.y;
    
    // Fill the triangle using horizontal scanlines
    float slope1 = dy12 != 0 ? dx12 / dy12 : 0;
    float slope2 = dy13 != 0 ? dx13 / dy13 : 0;
    float slope3 = dy23 != 0 ? dx23 / dy23 : 0;
    
    float x_start = p0.x;
    float x_end = p0.x;
    
    // Top half
    for (float y = p0.y; y <= p1.y; y++) {
        SDL_RenderLine(graphics::get().ren, x_start, y, x_end, y);
        x_start += slope1;
        x_end += slope2;
    }
    
    // Bottom half
    x_start = p1.x;
    for (float y = p1.y; y <= p2.y; y++) {
        SDL_RenderLine(graphics::get().ren, x_start, y, x_end, y);
        x_start += slope3;
        x_end += slope2;
    }
}

void draw_triangle(point p0, point p1, point p2, color color, float width) {
    draw_line(p0, p1, color, width);
    draw_line(p1, p2, color, width);
    draw_line(p2, p0, color, width);
}

scoped_graphics::scoped_graphics(const int screen_w, const int screen_h)
{
    init(screen_w, screen_h);
}

scoped_graphics::~scoped_graphics() { destroy(); }

void
scoped_graphics::init(const int screen_w, const int screen_h)
{
    if (!SDL_Init(0))
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!TTF_Init())
    {
        std::cout << __PRETTY_FUNCTION__ << ": TTF_Init error" << "\n";
        exit(EXIT_FAILURE);
    }

    graphics::get().win = SDL_CreateWindow("Timelines",
                                           screen_w,
                                           screen_h,
                                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);
    if (graphics::get().win == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    SDL_SetWindowFullscreen(graphics::get().win, 0);
    SDL_ShowCursor();

    graphics::get().ren = SDL_CreateRenderer(graphics::get().win, NULL);
    if (graphics::get().ren == nullptr)
    {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    SDL_SetRenderDrawBlendMode(graphics::get().ren, SDL_BLENDMODE_BLEND);

    graphics::get().bg = SDL_CreateTexture(graphics::get().ren,
                                           SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_TARGET,
                                           screen_w,
                                           screen_h);

    // TODO : paint background
    u8 grey = 0x30;
    SDL_SetRenderDrawColor(graphics::get().ren, grey, grey, grey, 0xFF);
    SDL_RenderClear(graphics::get().ren);
    SDL_RenderPresent(graphics::get().ren);
}

void
scoped_graphics::destroy()
{
    TTF_Quit();

    SDL_DestroyTexture(graphics::get().bg);
    SDL_DestroyRenderer(graphics::get().ren);
    SDL_DestroyWindow(graphics::get().win);
    SDL_Quit();
}
}
} // namespace tls
