#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "SDL2/SDL.h"
#include "Game.h"

typedef struct Food {
        int x;
        int y;
} Food;

/**
 * Moves the food to a new location
 */
void food_move(Food *food);

/**
 * Renders the food at the given location / point
 */
void food_render(SDL_Surface *window_surf, Food *food);
