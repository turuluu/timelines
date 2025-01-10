#pragma once

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

// TODO : Dirty global for experimentation
static struct Graphics
{
    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Texture* bg;
} g;

static void
clear()
{
    // TODO : paint background
    u8 grey = 0x30;

    u8 r, gr, b, a;
    SDL_GetRenderDrawColor(g.ren, &r, &gr, &b, &a);
    SDL_SetRenderDrawColor(g.ren, grey, grey, grey, 0xFF);
    SDL_RenderClear(g.ren);
    SDL_SetRenderDrawColor(g.ren, r, gr, b, a);
}

static TTF_Font*
get_title_font(int fontSize)
{
    auto font = TTF_OpenFont("../fonts/vera-fonts/dejavuSansMono.ttf", fontSize);
    assert(font);
    return font;
}

static void
render_text(TTF_Font* font,
            SDL_Color* color,
            SDL_Rect* msgBounds,
            const char* text,
            int ptsize = 40)
{
    if (font == nullptr || text == nullptr)
    {
        std::cout << "font was null. Exiting text rendering..";
        return;
    }

    SDL_Surface* msgSurface;
    if (!(msgSurface = TTF_RenderUTF8_Solid(font, text, *color)))
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << TTF_GetError << "\n";
        return;
    }

    const int ptsizeSmooth = ptsize / 2;
    const int cstrL = util::length(text);
    const int cstrW = cstrL * (ptsizeSmooth / 2);

    SDL_Rect msgBox;
    int margins = -1 * ptsizeSmooth;
    int offsetMsgBoundsW = msgBounds->w + (2 * margins);
    int offsetMsgBoundsX = msgBounds->x - offsetMsgBoundsW - margins;
    msgBox.w = std::min(cstrW, offsetMsgBoundsW);
    msgBox.h = ptsizeSmooth;
    msgBox.x = msgBounds->x - margins; // + std::max(offsetMsgBoundsW - cstrW, 0) / 2;
    msgBox.y = msgBounds->y + std::max(msgBounds->h - msgBox.h, 0) / 2;
    //        SDL_BlitSurface(msgSurface, NULL, screenSurface, msgBounds);
    SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(g.ren, msgSurface);
    SDL_RenderCopy(g.ren, msgTexture, NULL, &msgBox);

    SDL_DestroyTexture(msgTexture);
    SDL_FreeSurface(msgSurface);
}

static void
render_text_2(TTF_Font* font,
              SDL_Color* color,
              SDL_Rect* msgBounds,
              const char* text,
              int ptsize = 40)
{
    if (font == nullptr || text == nullptr)
    {
        std::cout << "font was null. Exiting text rendering..";
        return;
    }

    SDL_Surface* msgSurface;
    if (!(msgSurface = TTF_RenderUTF8_Solid(font, text, *color)))
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << TTF_GetError << "\n";
        return;
    }

    const int ptsizeSmooth = ptsize / 2;
    const int cstrL = util::length(text);
    const int cstrW = cstrL * (ptsizeSmooth / 2);

    SDL_Rect msgBox;
    int margins = 2 * ptsizeSmooth;
    int offsetMsgBoundsW = msgBounds->w + (2 * margins);
    int offsetMsgBoundsX = msgBounds->x - offsetMsgBoundsW - margins;
    msgBox.w = std::min(cstrW, offsetMsgBoundsW);
    msgBox.h = ptsizeSmooth;
    msgBox.x = msgBounds->x - margins + std::max(offsetMsgBoundsW - cstrW, 0) / 2;
    msgBox.y = msgBounds->y + std::max(msgBounds->h - msgBox.h, 0) / 2;
    //        SDL_BlitSurface(msgSurface, NULL, screenSurface, msgBounds);
    SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(g.ren, msgSurface);
    SDL_RenderCopy(g.ren, msgTexture, NULL, &msgBox);

    SDL_DestroyTexture(msgTexture);
    SDL_FreeSurface(msgSurface);
}

struct ScopedGraphics
{
    ScopedGraphics(const int screen_w, const int screen_h) { init(screen_w, screen_h); }
    ~ScopedGraphics() { destroy(); }

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

        g.win = SDL_CreateWindow("Timelines", 0, 0, screen_w, screen_h, SDL_WINDOW_SHOWN);
        if (g.win == nullptr)
        {
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }
        SDL_SetWindowFullscreen(g.win, 0);
        SDL_ShowCursor(1);

        g.ren = SDL_CreateRenderer(g.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (g.ren == nullptr)
        {
            std::cout << "SDL_CreateRenderer Error" << SDL_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }
        SDL_SetRenderDrawBlendMode(g.ren, SDL_BLENDMODE_BLEND);

        g.bg = SDL_CreateTexture(g.ren,
                                 SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET,
                                 screen_w,
                                 screen_h);

        // TODO : paint background
        u8 grey = 0x30;
        SDL_SetRenderDrawColor(g.ren, grey, grey, grey, 0xFF);
        SDL_RenderClear(g.ren);
        SDL_RenderPresent(g.ren);
    }

    void destroy()
    {
        TTF_Quit();

        SDL_DestroyTexture(g.bg);
        SDL_DestroyRenderer(g.ren);
        SDL_DestroyWindow(g.win);
        SDL_Quit();
    }
};
}
}