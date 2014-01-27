#include "Food.h"

/**
 * Moves the food to a new location
 */
void Food_Move(Food *food)
{
        srand(time(NULL));
        food->x = 12 * (rand() % 25);
        food->y = 12 * (rand() % 25);
}

/**
 * Renders the food at the given location / point
 */
void Food_Render(SDL_Surface *window_surf, Food *food)
{
        SDL_Surface *surf;
        SDL_Rect r;

        surf = SDL_CreateRGBSurface(0, 10, 10, 32,
                                    0xFF000000,
                                    0x00FF0000,
                                    0x0000FF00,
                                    0x000000FF);
        r.h = 8; r.w = 8;
        r.x = 2; r.y = 2;
        SDL_FillRect(surf, &r, 0x66666666);

        r.h = 8; r.w = 8;
        r.x = 0; r.y = 0;
        SDL_FillRect(surf, &r, 0x666666FF);

        r.w = 10; r.h = 10;
        r.x = food->x;
        r.y = food->y;

        SDL_BlitSurface(surf, NULL, window_surf, &r);

        SDL_FreeSurface(surf);
}
