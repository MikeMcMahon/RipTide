#pragma once

#include "SDL2/SDL.h"

SDL_Rect SDL_CreateRect(int, int, int, int);
void SDL_GridFilter(Uint8 *, int, Uint8, int, int, int);
