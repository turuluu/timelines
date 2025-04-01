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

static void
clear()
{
    // TODO : paint background
    u8 grey = 0x30;

    u8 r, gr, b, a;
    SDL_GetRenderDrawColor(graphics::get().ren, &r, &gr, &b, &a);
    SDL_SetRenderDrawColor(graphics::get().ren, grey, grey, grey, 0xFF);
    SDL_RenderClear(graphics::get().ren);
    SDL_SetRenderDrawColor(graphics::get().ren, r, gr, b, a);
}
font* get_title_font(int font_size);
void destroy_font(font* font);
void render_text(font* font, SDL_Color* color, rect* msg_bounds, const char* text, int ptsize = 40);

void render_text_2(font* font,
                   SDL_Color* color,
                   rect* msg_bounds,
                   const char* text,
                   int ptsize = 40);

struct scoped_graphics
{
    scoped_graphics(const int screen_w, const int screen_h);
    ~scoped_graphics();

    void init(const int screen_w, const int screen_h);
    void destroy();
};
}

} // namespace tls