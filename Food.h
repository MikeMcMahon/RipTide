#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "Game.h"

struct _Food * Food_Create();
void Food_Free(Food *);

/**
 * Moves the food to a new location
 */
void Food_Move(Food *, Snake *);
