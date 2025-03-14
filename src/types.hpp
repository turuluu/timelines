#pragma once

// #include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_rect.h>

namespace tls
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

using rect = SDL_FRect;

struct colour
{
    u8 border = 255;
    u8 fill = 255;
};

struct mouse_move
{
    float x;
    float y;
};

}