#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "Game.h"

/**
 * Moves the food to a new location
 */
void Food_Move(Food *);

/**
 * Renders the food at the given location / point
 */
void Food_Render(SDL_Surface *, Food *);
