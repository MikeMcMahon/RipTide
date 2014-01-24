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
        unsigned int width;
        unsigned int height;
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
        food->x = (rand() % (game_window.width - 10));
        food->y = (rand() % (game_window.height - 10));
}

/**
 * Renders the food at the given location / point
 */
void food_render(SDL_Renderer *renderer, Food *food)
{
        SDL_Surface *surf;
        SDL_Texture *text;
        SDL_Rect rect;
        rect = SDL_CreateRect(10, 10, food->x, food->y);
        surf = SDL_CreateRGBSurface(
                       0,
                       10,
                       10,
                       32,
                       0xFF000000, //rmask
                       0x00FF0000, //bmask
                       0x0000FF00, //gmask
                       0x000000FF);//amask
        SDL_FillRect(surf, NULL, 0x33AA33FF);
        text = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_RenderCopy(renderer, text, NULL, &rect);
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
        Snake_Move dir;
} Body;

/**
 * The game snake itself
 */
typedef struct Snake {
        int size;
        struct Body *body;
        SDL_Surface *surf;
        SDL_Texture *texture;
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
        new_body->dir = SNAKE_LEFT;
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
        snake->surf = SDL_CreateRGBSurface(
                              0,
                              10,
                              10,
                              32,
                              0xFF000000, //rmask
                              0x00FF0000, //bmask
                              0x0000FF00, //gmask
                              0x000000FF);//amask
        SDL_FillRect(snake->surf, NULL, 0xAAAAAAFF);
        snake->texture = SDL_CreateTextureFromSurface(renderer, snake->surf);
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

        if (body->value.x > game_window.width && dir == SNAKE_RIGHT)
                body->value.x = 0;
        if (body->value.x < 0 && dir == SNAKE_LEFT)
                body->value.x = game_window.width;

        if (body->value.y > game_window.height && dir == SNAKE_DOWN)
                body->value.y = 0;
        if (body->value.y < 0 && dir == SNAKE_UP)
                body->value.y = game_window.height;
}

void snake_handle_input(Snake *snake, Snake_Move dir, int incr)
{
        Body *body = snake->body->next;
        //snake->body->dir = dir;

        body = snake_get_tail(snake->body);
        while (body != NULL) {
                printf("((%d,%d)%d),", body->value.x, body->value.y, body->dir);

                if (body->prev != NULL) {
                        body->value.x = body->prev->value.x;
                        body->value.y = body->prev->value.y;
                        //body->dir = body->prev->dir;
                } else {
                        snake_move(body, dir, incr);
                }
                body = body->prev;
        }
        printf("\r\n");
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
        Snake_Move prev_dir = body->dir;
        snake_handle_input(snake, dir, incr);
        SDL_Rect r = SDL_CreateRect(10, 10, x, y);
        snake_init_body(&body->next, &r, body);
        body->next->dir = prev_dir;
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

        Food food;

        game_window.width = DEFAULT_WIDTH;
        game_window.height = DEFAULT_HEIGHT;

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

        if (SDL_CreateWindowAndRenderer(
                    game_window.width,
                    game_window.height,
                            SDL_WINDOWPOS_CENTERED |
                            SDL_WINDOW_OPENGL |
                            SDL_WINDOW_INPUT_GRABBED,
                    &window, &renderer) != 0)
                return 2;

        SDL_SetWindowTitle(window, "RipTide");

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

                for (int i = 1; i <= snake.size; i++) {
                        draw_rect = *snake_body_at(&snake, i);
                        SDL_RenderCopy(renderer, snake.texture, NULL, &draw_rect);
                }

                food_render(renderer, &food);

                SDL_RenderPresent(renderer);
        }

        snake_free(&snake);

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
