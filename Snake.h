#pragma once

#include "SDL2/SDL.h"

#include "Game.h"
#include "Food.h"


/**
 * Defines the current state of the snake
 */
typedef enum {
        SNAKE_DYING = 0x0001,
        SNAKE_ALIVE = 0x0010
} Snake_State;

/**
 * Defines the current direction the snake is moving
 */
typedef enum {
        SNAKE_LEFT = 1,
        SNAKE_RIGHT,
        SNAKE_UP,
        SNAKE_DOWN
} Snake_Direction;

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
        int increment;
        struct Body *body;
        SDL_Surface *surf;
        Snake_State state;
} Snake;

void Snake_SetScene(struct Scene **);

struct Snake * Snake_Create(SDL_Renderer *renderer);
void Snake_InitBody(Body **out_body, SDL_Rect *rect, Body *prev);
int Snake_Free();

SDL_Rect *Snake_BodyAt(Snake *snake, int pos);
Body * Snake_GetTail(Body *body);
void Snake_PopTail();

void Snake_Move(Body *body, Snake_Direction dir, int incr);
void Snake_MoveNext(Snake_Direction dir, SDL_Rect *out_rect);
void Snake_TranslateBody(SDL_Rect * body, Snake_Direction dir);

void Snake_HandleInput(Snake_Direction dir);

void Snake_Nibble(Snake_Direction dir);
int Snake_IntersectFood(SDL_Rect *head, Food *food);
int Snake_IntersectSelf(SDL_Rect *next_move);
