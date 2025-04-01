#pragma once

/**
 * The idea is to be able to include multiple backends to compare.
 * This is hardly necessary, but it's here for trying out inline namespaces and
 * maybe for fun later.
 */
#if USE_SDL
#include "sdl/graphics.hpp"
#include "sdl/events.hpp"
#else
#error "Only SDL backend is currently implemented"
#endif