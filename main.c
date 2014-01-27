#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include "SDL_Util.h"
#include "Snake.h"
#include "Food.h"

#define DEFAULT_WIDTH 312
#define DEFAULT_HEIGHT 312

struct Scene *scene;

struct Scene * Scene_Create(int w, int h)
{
        struct Scene * ascene = malloc(sizeof(struct Scene));
        ascene->h = h;
        ascene->w = w;
        return ascene;
};

/******************** SDL ******************/

void update(Snake *, Snake_Direction *, Food *);
void render(SDL_Renderer *, SDL_Texture *, SDL_Surface *, float, Snake *, Food *);
int input(SDL_Event *e, Snake_Direction *);

int main (int argc, char *args[])
{
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_bool quit = SDL_FALSE;
        SDL_Event e;
        SDL_Surface *window_surface;
        SDL_Surface *font_surface;
        SDL_Texture *game_texture;
        TTF_Font *font;

        Food food;


        food_move(&food);

        /** TIME KEEPING **/
        unsigned int frames_per_second = 60;
        unsigned int ms_per_update = 1000 / frames_per_second;
        unsigned int previous = SDL_GetTicks();
        unsigned int current = 0;
        unsigned int lag = 0;
        unsigned int elapsed = 0;
        float interpolation;
        /** TIME KEEPING **/

        Snake *snake;
        Snake_Direction cur_dir = SNAKE_LEFT;
        float snake_fps = 10;
        float snake_movement = 12;
        float snake_ms_per_update = (1000 / snake_fps);
        unsigned int snake_lag = 0;

        scene = Scene_Create(DEFAULT_WIDTH, DEFAULT_HEIGHT);

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
                return 1;

        window = SDL_CreateWindow("RipTide",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  scene->w, scene->h,
                                  SDL_WINDOW_SHOWN);
        if (window == NULL) {
                printf("ERROR CREATING WINDOW: %s", SDL_GetError());
                return 1;
        }

        renderer = SDL_CreateRenderer(window,
                                      -1,
                                      SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) {
                printf("ERROR CREATING RENDERER: %s", SDL_GetError());
                return 1;
        }


        SDL_SetWindowTitle(window, "RipTide");

        window_surface = SDL_GetWindowSurface(window);
        game_texture = SDL_CreateTextureFromSurface(renderer, window_surface);

        snake = Snake_Create(renderer);
        Snake_SetScene(&scene);
        snake->increment = snake_movement;
        snake->state = SNAKE_ALIVE;

        if (TTF_Init() != 0)
                return 1;

        font = TTF_OpenFont(
                "D:\\Development\\dependencies\\fonts\\November.ttf",
                28);

        if (font == NULL)
                return 1;

        SDL_Color fg;
        fg.a = 0xFF;
        fg.r = 0x66;
        fg.g = 0x66;
        fg.b = 0x66;
        font_surface = TTF_RenderText_Blended(font, "SnAkE", fg);

        SDL_Rect font_location;
        font_location.w = font_surface->w;
        font_location.h = font_surface->h;
        font_location.x = (DEFAULT_WIDTH / 2) - (font_location.w / 2);
        font_location.y = 0;

        while (!quit) {
                current = SDL_GetTicks();
                elapsed = current - previous;
                previous = current;
                lag += elapsed;

                SDL_PollEvent(&e);
                if (input(&e, &cur_dir) != 0)
                        quit = SDL_TRUE;

                // Game runs at 60FPS snake runs at 10FPS
                while (lag >= ms_per_update) {
                        snake_lag += ms_per_update;

                        if (snake_lag >= snake_ms_per_update) {
                                update(snake, &cur_dir, &food);
                                snake_lag -= snake_ms_per_update;
                        }
                        lag -= ms_per_update;
                }

                interpolation = (float)lag / (float)ms_per_update;
                render(renderer, game_texture, window_surface, interpolation, snake, &food);
                SDL_BlitSurface(font_surface, NULL, window_surface, &font_location);
                SDL_UpdateTexture(game_texture, NULL,
                                  window_surface->pixels,
                                  window_surface->pitch);
                SDL_RenderCopy(renderer, game_texture, NULL, NULL);

                SDL_RenderPresent(renderer);
        }


        Snake_Free();

        TTF_Quit();

        SDL_FreeSurface(snake->surf);
        SDL_DestroyTexture(game_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}

void render(SDL_Renderer *renderer, SDL_Texture *game_texture,
            SDL_Surface *window_surface, float interpolation,
            Snake *snake, Food *food) {
        SDL_Rect bounds;
        SDL_RenderClear(renderer);

        SDL_FillRect(window_surface, NULL, 0xFFBECFAE);

        for (int i = 1; i <= snake->size; i++) {
                bounds = *Snake_BodyAt(snake, i);
                SDL_BlitSurface(snake->surf, NULL, window_surface, &bounds);
        }

        food_render(window_surface, food);
}

void update(Snake *snake, Snake_Direction *cur_dir, Food *food) {
        SDL_Rect next_move;
        SDL_Rect new_body;

        if (snake->state == SNAKE_ALIVE) {
                Snake_MoveNext(*cur_dir, &next_move);
                if (Snake_IntersectFood(&next_move, food) == 0) {
                        food_move(food);
                        Snake_Nibble(*cur_dir);
                }

                if (Snake_IntersectSelf(&next_move) == 0) {
                        snake->state = SNAKE_DYING;
                } else {
                        Snake_HandleInput(*cur_dir);
                }
        } else if (snake->state == SNAKE_DYING) {
                if (snake->size > 0) {
                        Snake_PopTail(snake);
                } else if (snake->size == 0) {
                        new_body = SDL_CreateRect(10, 10, 156, 156);
                        Snake_InitBody(&snake->body, &new_body, NULL);
                        snake->state = SNAKE_ALIVE;
                        snake->size++;
                        *cur_dir = SNAKE_LEFT;
                }
        }
}

/**
 * Handles the input from the game and returns -1 if should quit
 * otherwise a zero that input was handled per usual
 */
int input(SDL_Event *e, Snake_Direction *direction)
{
        if (e->type == SDL_QUIT)
                return -1;

        if (e->type == SDL_KEYDOWN) {
                if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE] != 0)
                        return -1;
                if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP] != 0) {
                        *direction = SNAKE_UP;
                }
                if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT] != 0) {
                        *direction = SNAKE_RIGHT;
                }
                if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT] != 0) {
                        *direction = SNAKE_LEFT;
                }
                if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN] != 0) {
                        *direction = SNAKE_DOWN;
                }
        }

        return 0;
}
