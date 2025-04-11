#pragma once

#include <cassert>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// incrementally decoupling gfx lib

#if USE_SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#endif

#include "../types.hpp"
#include "SDL3_ttf/SDL_ttf.h"

namespace tls
{
#if USE_SDL
inline
#endif
  namespace sdl
{
using font = TTF_Font;
struct graphics
{
    static graphics& get()
    {
        static graphics instance;
        return instance;
    }

    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Texture* bg;
};

void clear();
void destroy_font(font* font);
font* get_title_font(int font_size);

void render_text(font* font, const color* color, rect* msg_bounds, const char* text, int ptsize = 40);

void render_text_2(font* font,
                   const color* color,
                   rect* msg_bounds,
                   const char* text,
                   int ptsize = 40);

void draw_filled_rect(rect rect, color color);
void draw_rect(rect rect, color color);
void draw_filled_circle(float x, float y, float radius, color color);
void draw_circle(float x, float y, float radius, color color);
void draw_line(point p0, point p1, color color, float width = 1.0f);
void draw_filled_triangle(point p0, point p1, point p2, color color);
void draw_triangle(point p0, point p1, point p2, color color, float width = 1.0f);

struct scoped_graphics
{
    scoped_graphics(int screen_w, int screen_h);
    ~scoped_graphics();

    void init(int screen_w, int screen_h);
    void destroy();
};
}

} // namespace tls