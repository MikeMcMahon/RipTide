#include "Food.h"

/**
 * Generate food x,y coordinates
 */
void Food_GenerateLoc(Food *food)
{
        food->x = 10 * (rand() % 25);
        food->y = 10 * (rand() % 25);
        food->bounds.x = food->x;
        food->bounds.y = food->y;}

/**
 * Creates the food
 */
struct _Food * Food_Create()
{
        SDL_Rect r;
        Food *food = malloc(sizeof(struct _Food));
        Food_GenerateLoc(food);

        food->bounds.w = 10;
        food->bounds.h = 10;

        food->surf = SDL_CreateRGBSurface(0, 10, 10, 32,
                                    0xFF000000,
                                    0x00FF0000,
                                    0x0000FF00,
                                    0x000000FF);
        r.h = 10; r.w = 10;
        r.x = 0; r.y = 0;
        SDL_FillRect(food->surf, &r, 0x666666FF);

        SDL_GridFilter(food->surf->pixels, food->surf->pitch, 0x77, 10, 10, 4);


        return food;
}

/**
 * Cleans up the food lol
 */
void Food_Free(Food *food)
{
        SDL_FreeSurface(food->surf);
        free(food);
}


/**
 * Moves the food to a new location
 */
void Food_Move(Food *food, Snake *snake)
{
        Food_GenerateLoc(food);
        struct _Body *body = (snake)->body;
        while (body != NULL) {
                if (body->value.x == food->x && body->value.y == food->y) {
                        Food_Move(food, snake);
                        break;
                }
                body = body->next;
        }
}
