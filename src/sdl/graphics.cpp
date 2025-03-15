#include "graphics.hpp"

#include <SDL3_ttf/SDL_ttf.h>

namespace tls
{

#if USE_SDL
inline
#endif
  namespace sdl
{

TTF_Font*
get_title_font(int font_size)
{
    auto font = TTF_OpenFont("../fonts/vera-fonts/dejavuSansMono.ttf", font_size);
    assert(font);
    return font;
}

void
destroy_font(TTF_Font* font)
{
    if (font != nullptr)
        TTF_CloseFont(font);
}

void
render_text(TTF_Font* font, SDL_Color* color, rect* msg_bounds, const char* text, int ptsize)
{
    if (font == nullptr || text == nullptr)
    {
        std::cout << "font was null. Exiting text rendering..";
        return;
    }

    SDL_Surface* msg_surface;
    if (!(msg_surface = TTF_RenderText_Blended(font, text, strlen(text), *color)))
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << SDL_GetError() << "\n";
        return;
    }

    const int pt_size_smooth = ptsize / 2;
    const int cstrL = util::length(text);
    const int cstrW = cstrL * (pt_size_smooth / 2);

    SDL_FRect msg_box;
    int margins = -1 * pt_size_smooth;
    int offset_msg_bounds_w = msg_bounds->w + (2 * margins);
    int offsetMsgBoundsX = msg_bounds->x - offset_msg_bounds_w - margins;
    msg_box.w = std::min(cstrW, offset_msg_bounds_w);
    msg_box.h = pt_size_smooth;
    msg_box.x = msg_bounds->x - margins; // + std::max(offset_msg_bounds_w - cstrW, 0) / 2;
    msg_box.y = msg_bounds->y + std::max((float)msg_bounds->h - msg_box.h, 0.0f) / 2;
    SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(graphics::get().ren, msg_surface);
    SDL_RenderTexture(graphics::get().ren, msg_texture, NULL, &msg_box);

    SDL_DestroyTexture(msg_texture);
    SDL_DestroySurface(msg_surface);
}
void
render_text_2(TTF_Font* font, SDL_Color* color, rect* msg_bounds, const char* text, int ptsize)
{
    if (font == nullptr || text == nullptr)
    {
        std::cout << "font was null. Exiting text rendering..";
        return;
    }

    SDL_Surface* msg_surface;
    if (!(msg_surface = TTF_RenderText_Blended(font, text, strlen(text), *color)))
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << SDL_GetError() << "\n";
        return;
    }

    const int pt_size_smooth = ptsize / 2;
    const int cstrL = util::length(text);
    const int cstrW = cstrL * (pt_size_smooth / 2);

    SDL_FRect msgBox;
    int margins = 2 * pt_size_smooth;
    int offset_msg_bounds_w = msg_bounds->w + (2 * margins);
    int offsetMsgBoundsX = msg_bounds->x - offset_msg_bounds_w - margins;
    msgBox.w = std::min(cstrW, offset_msg_bounds_w);
    msgBox.h = pt_size_smooth;
    msgBox.x = msg_bounds->x - margins + std::max(offset_msg_bounds_w - cstrW, 0) / 2;
    msgBox.y = msg_bounds->y + std::max((float)msg_bounds->h - msgBox.h, 0.0f) / 2;
    //        SDL_BlitSurface(msg_surface, NULL, screenSurface, msgBounds);
    SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(graphics::get().ren, msg_surface);
    SDL_RenderTexture(graphics::get().ren, msg_texture, NULL, &msgBox);

    SDL_DestroyTexture(msg_texture);
    SDL_DestroySurface(msg_surface);
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
