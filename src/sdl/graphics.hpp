#pragma once

#include <cassert>

// incrementally decoupling gfx lib
#define USE_SDL 1

#if USE_SDL
#include <SDL_ttf.h>

#include <SDL.h>
#include <SDL_mouse.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#endif

//#ifdef __EMSCRIPTEN__
// #include <emscripten.h>
//#endif

#include <iostream>

#include "../utilities.hpp"

namespace tls
{

#if USE_SDL
inline
#endif

namespace sdl
{
// Using SDL types
using i8 = Sint8;
using i16 = Sint16;
using i32 = Sint32;
using i64 = Sint64;

using u8 = Uint8;
using u16 = Uint16;
using u32 = Uint32;
using u64 = Uint64;

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

static TTF_Font*
get_title_font(int font_size)
{
    auto font = TTF_OpenFont("../fonts/vera-fonts/dejavuSansMono.ttf", font_size);
    assert(font);
    return font;
}

static void
render_text(TTF_Font* font,
            SDL_Color* color,
            SDL_Rect* msg_bounds,
            const char* text,
            int ptsize = 40)
{
    if (font == nullptr || text == nullptr)
    {
        std::cout << "font was null. Exiting text rendering..";
        return;
    }

    SDL_Surface* msg_surface;
    if (!(msg_surface = TTF_RenderUTF8_Solid(font, text, *color)))
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << TTF_GetError << "\n";
        return;
    }

    const int pt_size_smooth = ptsize / 2;
    const int cstrL = util::length(text);
    const int cstrW = cstrL * (pt_size_smooth / 2);

    SDL_Rect msg_box;
    int margins = -1 * pt_size_smooth;
    int offset_msg_bounds_w = msg_bounds->w + (2 * margins);
    int offsetMsgBoundsX = msg_bounds->x - offset_msg_bounds_w - margins;
    msg_box.w = std::min(cstrW, offset_msg_bounds_w);
    msg_box.h = pt_size_smooth;
    msg_box.x = msg_bounds->x - margins; // + std::max(offset_msg_bounds_w - cstrW, 0) / 2;
    msg_box.y = msg_bounds->y + std::max(msg_bounds->h - msg_box.h, 0) / 2;
    //        SDL_BlitSurface(msg_surface, NULL, screenSurface, msgBounds);
    SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(graphics::get().ren, msg_surface);
    SDL_RenderCopy(graphics::get().ren, msg_texture, NULL, &msg_box);

    SDL_DestroyTexture(msg_texture);
    SDL_FreeSurface(msg_surface);
}

static void
render_text_2(TTF_Font* font,
              SDL_Color* color,
              SDL_Rect* msg_bounds,
              const char* text,
              int ptsize = 40)
{
    if (font == nullptr || text == nullptr)
    {
        std::cout << "font was null. Exiting text rendering..";
        return;
    }

    SDL_Surface* msg_surface;
    if (!(msg_surface = TTF_RenderUTF8_Solid(font, text, *color)))
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << TTF_GetError << "\n";
        return;
    }

    const int pt_size_smooth = ptsize / 2;
    const int cstrL = util::length(text);
    const int cstrW = cstrL * (pt_size_smooth / 2);

    SDL_Rect msgBox;
    int margins = 2 * pt_size_smooth;
    int offset_msg_bounds_w = msg_bounds->w + (2 * margins);
    int offsetMsgBoundsX = msg_bounds->x - offset_msg_bounds_w - margins;
    msgBox.w = std::min(cstrW, offset_msg_bounds_w);
    msgBox.h = pt_size_smooth;
    msgBox.x = msg_bounds->x - margins + std::max(offset_msg_bounds_w - cstrW, 0) / 2;
    msgBox.y = msg_bounds->y + std::max(msg_bounds->h - msgBox.h, 0) / 2;
    //        SDL_BlitSurface(msg_surface, NULL, screenSurface, msgBounds);
    SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(graphics::get().ren, msg_surface);
    SDL_RenderCopy(graphics::get().ren, msg_texture, NULL, &msgBox);

    SDL_DestroyTexture(msg_texture);
    SDL_FreeSurface(msg_surface);
}

struct scoped_graphics
{
    scoped_graphics(const int screen_w, const int screen_h) { init(screen_w, screen_h); }
    ~scoped_graphics() { destroy(); }

    void init(const int screen_w, const int screen_h)
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }

        if (TTF_Init() == -1)
        {
            printf("TTF_Init: %s\n", TTF_GetError());
            exit(2);
        }

        graphics::get().win = SDL_CreateWindow("Timelines", 0, 0, screen_w, screen_h, SDL_WINDOW_SHOWN);
        if (graphics::get().win == nullptr)
        {
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }
        SDL_SetWindowFullscreen(graphics::get().win, 0);
        SDL_ShowCursor(1);

        graphics::get().ren = SDL_CreateRenderer(graphics::get().win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (graphics::get().ren == nullptr)
        {
            std::cout << "SDL_CreateRenderer Error" << SDL_GetError() << std::endl;
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

    void destroy()
    {
        TTF_Quit();

        SDL_DestroyTexture(graphics::get().bg);
        SDL_DestroyRenderer(graphics::get().ren);
        SDL_DestroyWindow(graphics::get().win);
        SDL_Quit();
    }
};
}
}