#pragma once

#if USE_SDL
#include <SDL3/SDL_rect.h>
#endif

namespace tls
{
#if USE_SDL
inline
#endif
  namespace sdl
{
using i8 = Sint8;
using i16 = Sint16;
using i32 = Sint32;
using i64 = Sint64;

using u8 = Uint8;
using u16 = Uint16;
using u32 = Uint32;
using u64 = Uint64;

using rect = SDL_FRect;
using point = SDL_FPoint;
}
} // namespace tls
