#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "SDL2/SDL.h"

#include "SDL_Util.h"
#include "Snake.h"

#define DEFAULT_WIDTH 300
#define DEFAULT_HEIGHT 300

typedef struct {
        unsigned int w;
        unsigned int h;
} GameWindow;

GameWindow game_window;

/****************** FOOD *************************/

typedef struct Food {
        int x;
        int y;
} Food;

/**
 * Moves the food to a new location
 */
void food_move(Food *food)
{
        srand(time(NULL));
        food->x = (rand() % (game_window.w - 10));
        food->y = (rand() % (game_window.h - 10));
}

/**
 * Renders the food at the given location / point
 */
void food_render(SDL_Surface *window_surf, Food *food)
{
        SDL_Surface *surf;
        SDL_Texture *text;
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
}

/****************** SNAKE ************************/

typedef enum {
        SNAKE_LEFT = 1,
        SNAKE_RIGHT,
        SNAKE_UP,
        SNAKE_DOWN
} Snake_Move;

/**
 * Linked list of snake body parts
 */
typedef struct Body {
        struct Body *next;
        struct Body *prev;
        SDL_Rect value;
} Body;

/**
 * The game snake itself
 */
typedef struct Snake {
        int size;
        struct Body *body;
        SDL_Surface *surf;
} Snake;

/**
 * Initializes a snake body part
 */
void snake_init_body(Body **out_body, SDL_Rect *rect, Body *prev)
{
        struct Body* new_body = malloc(sizeof(struct Body));
        new_body->next = NULL;
        new_body->prev = prev;
        new_body->value = *rect;
        *out_body = new_body;
}

/**
 * Creates a new snake
 */
void snake_create(SDL_Renderer *renderer, Snake *snake)
{
        SDL_Rect r = SDL_CreateRect(10, 10, 100, 100);

        snake->size = 1;
        snake_init_body(&snake->body, &r, NULL);
        snake->surf = SDL_CreateRGBSurface(0, 10, 10, 32,
                                           0xFF000000,
                                           0x00FF0000,
                                           0x0000FF00,
                                           0x000000FF);
        SDL_FillRect(snake->surf, NULL, 0x00000000);

        r.h = 8;
        r.w = 8;
        r.x = 2;
        r.y = 2;
        SDL_FillRect(snake->surf, &r, 0x66666677);

        r.h = 8;
        r.w = 8;
        r.x = 0;
        r.y = 0;
        SDL_FillRect(snake->surf, &r, 0x666666FF);

        /*Uint8 * pixel;
        Uint8 alpha = 255;
        for (int x = 3; x >= 0; x--) {
                alpha = 255 * ((float)(3 * x) / 10.0);
                for (int y = 0; y < 10; y++) {
                        pixel = (Uint8*)snake->surf->pixels;
                        pixel += (y * snake->surf->pitch) + (x * sizeof(Uint32));
                        *pixel = alpha;
                        printf("%d %d %d %d\r\n", *pixel, pixel[sizeof(Uint8)*1],
                               pixel[sizeof(Uint8)*2], pixel[sizeof(Uint8)*3]);
                }
        }

        for (int x = 10; x >= 7; x--) {
                alpha = 255 * ((float)(3 * (10 - x)) / 10.0);
                for (int y = 0; y < 10; y++) {
                        pixel = (Uint8*)snake->surf->pixels;
                        pixel += (y * snake->surf->pitch) + (x * sizeof(Uint32));
                        *pixel = alpha;
                        printf("%d %d %d %d\r\n", *pixel, pixel[sizeof(Uint8)*1],
                               pixel[sizeof(Uint8)*2], pixel[sizeof(Uint8)*3]);
                }
        }*/
}

/**
 * Gets the tail of the snake by traversing its body
 */
Body * snake_get_tail(Body *body)
{
        Body *abody = body;
        while (abody->next != NULL) {
                abody = abody->next;
        }

        return abody;
}

/**
 * Gets the rectangle represented at a specific point of the body
 * or the most recently found piece if outside the bounds of the tail...
 */
SDL_Rect *snake_body_at(Snake *snake, int pos)
{
        Body *abody = snake->body;
        SDL_Rect *rect = NULL;

        for (int c = 0; c < pos; c++) { // hehe
                if (abody != NULL)
                        rect = &abody->value;

                if (abody->next != NULL)
                        abody = abody->next;
        }

        return rect;
}

void snake_move(Body *body, Snake_Move dir, int incr)
{
        int x = 0;
        int y = 0;
        switch(dir) {
        case SNAKE_LEFT:
                x -= incr;
                break;
        case SNAKE_UP:
                y -= incr;
                break;
        case SNAKE_RIGHT:
                x += incr;
                break;
        case SNAKE_DOWN:
                y += incr;
                break;
        }

        body->value.x += x;
        body->value.y += y;

        if (body->value.x >= game_window.w && dir == SNAKE_RIGHT)
                body->value.x = (body->value.x - game_window.w);

        if (body->value.x <= 0 && dir == SNAKE_LEFT)
                body->value.x = game_window.w + body->value.x;

        if (body->value.y >= game_window.h && dir == SNAKE_DOWN)
                body->value.y = body->value.y - game_window.h;

        if (body->value.y <= 0 && dir == SNAKE_UP)
                body->value.y = game_window.h + body->value.y;
}

void snake_handle_input(Snake *snake, Snake_Move dir, int incr)
{
        Body *body = snake->body->next;

        body = snake_get_tail(snake->body);
        while (body != NULL) {
#ifdef DEBUG
                printf("(%d,%d),", body->value.x, body->value.y);
#endif

                if (body->prev != NULL) {
                        body->value.x = body->prev->value.x;
                        body->value.y = body->prev->value.y;
                } else {
                        snake_move(body, dir, incr);
                }
                body = body->prev;
        }
#ifdef DEBUG
        printf("\r\n");
#endif
}

/**
 * Increases the length of the snake by one,
 * accomplished by moving the snake forward one whole iteration
 * and then adding a new body piece at the end
 **/
void snake_nibble(Snake *snake, int incr,
                  Snake_Move dir)
{
        Body *body = snake_get_tail(snake->body);
        int x = body->value.x;
        int y = body->value.y;
        snake_handle_input(snake, dir, incr);
        SDL_Rect r = SDL_CreateRect(10, 10, x, y);
        snake_init_body(&body->next, &r, body);
        snake->size++;
}

/**
 * frees all memory allocated to the snake
 */
int snake_free(Snake *snake)
{
        struct Body *abody = snake->body;

        while (abody != NULL) {
                if (abody->next == NULL) {
                        free(abody);
                        break;
                } else {
                        abody = abody->next;
                }

                if (abody != NULL)
                        free(abody->prev);
        }

        return 0;
}

/**
 * Returns 0 if the snake collides with food, otherwise -1
 */
int snake_collides(Snake *snake, Food *food)
{
        SDL_Rect rect;
        rect.x = food->x;
        rect.y = food->y;
        rect.h = 10;
        rect.w = 10;

        if (SDL_HasIntersection(&snake->body->value, &rect) == SDL_TRUE) {
                return 0;
        }

        return -1;
}

/******************** SDL ******************/
void update();
void render(float interpolation);
int input(SDL_Event *e, Snake_Move *);

int main (int argc, char *args[])
{
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_bool quit = SDL_FALSE;
        SDL_Event e;
        SDL_Rect draw_rect; //  = malloc(sizeof(struct SDL_Rect));
        SDL_Surface *window_surface;
        SDL_Texture *game_texture;

        Food food;

        game_window.w = DEFAULT_WIDTH;
        game_window.h = DEFAULT_HEIGHT;

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

        Snake snake;
        Snake_Move cur_dir = SNAKE_LEFT;
        float snake_fps = 10;
        float snake_movement = 12;
        float snake_ms_per_update = (1000 / snake_fps);
        unsigned int snake_lag = 0;

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
                return 1;

        window = SDL_CreateWindow("RipTide",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  game_window.w, game_window.h,
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

        snake_create(renderer, &snake);

        while (!quit) {
                current = SDL_GetTicks();
                elapsed = current - previous;
                previous = current;
                lag += elapsed;

                SDL_PollEvent(&e);
                if (input(&e, &cur_dir) != 0)
                        quit = SDL_TRUE;

                while (lag >= ms_per_update) {
                        snake_lag += ms_per_update;

                        // Game runs at 60FPS snake runs at 10FPS
                        if (snake_lag >= snake_ms_per_update) {
                                snake_handle_input(
                                        &snake,
                                        cur_dir,
                                        snake_movement);

                                if (snake_collides(&snake, &food) == 0) {
                                        food_move(&food);
                                        snake_nibble(
                                                &snake,
                                                snake_movement,
                                                cur_dir);
                                }
                                snake_lag -= snake_ms_per_update;
                        }
                        lag -= ms_per_update;
                }

                interpolation = (float)lag / (float)ms_per_update;

                SDL_RenderClear(renderer);
                SDL_FillRect(window_surface, NULL, 0xFFBECFAE);

                for (int i = 1; i <= snake.size; i++) {
                        draw_rect = *snake_body_at(&snake, i);
                        SDL_BlitSurface(snake.surf, NULL, window_surface, &draw_rect);

                }

                food_render(window_surface, &food);
                SDL_UpdateTexture(game_texture, NULL, window_surface->pixels, window_surface->pitch);
                SDL_RenderCopy(renderer, game_texture, NULL, NULL);

                SDL_RenderPresent(renderer);
        }


        snake_free(&snake);

        SDL_DestroyTexture(game_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}

/**
 * Handles the input from the game and returns -1 if should quit
 * otherwise a zero that input was handled per usual
 */
int input(SDL_Event *e, Snake_Move *direction)
{
        if (e->type == SDL_QUIT)
                return -1;

        if (e->type == SDL_KEYDOWN) {
                if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE] != 0)
                        return -1;
                else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP] != 0)
                        *direction = SNAKE_UP;
                else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT] != 0)
                        *direction = SNAKE_RIGHT;
                else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT] != 0)
                        *direction = SNAKE_LEFT;
                else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN] != 0)
                        *direction = SNAKE_DOWN;
        }

        return 0;
}
