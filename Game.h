#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct Scene{
        unsigned int w;
        unsigned int h;
}Scene;

struct Scene * Scene_Create(int, int);

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

/** Forward Declarations **/
typedef struct _Snake Snake;
typedef struct _Food Food;
typedef struct _Body Body;

/**
 * Linked list of snake body parts
 */
struct _Body {
        struct _Body *next;
        struct _Body *prev;
        SDL_Rect value;
};

/**
 * The game snake itself
 */
struct _Snake {
        int size;
        int increment;
        struct _Body *body;
        SDL_Surface *surf;
        Snake_State state;
};

/**
 * The coordinates of the food to be consumed
 */
struct _Food {
        int x;
        int y;
};
