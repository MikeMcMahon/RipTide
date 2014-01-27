#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "SDL2/SDL.h"
#include "SDL_Util.h"

struct Scene{
        unsigned int w;
        unsigned int h;
};

struct Scene * Scene_Create(int, int);
