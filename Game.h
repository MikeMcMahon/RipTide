#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


/**
 * Defines the current state 0of the game
 */
typedef enum {
        GAME_START_SCREEN = 0x1,
        GAME_IN_GAME,
        GAME_PAUSE
} GameState;

typedef struct _Scene{
        int w;
        int h;
        GameState state;
        SDL_Window *window;
        SDL_Surface *window_surface;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
}Scene;

int Scene_Create(Scene **, int, int);

/**
 * Defines the current state of the snake
 */
typedef enum {
        SNAKE_DYING = 0x0001,
        SNAKE_ALIVE = 0x0010,
        SNAKE_EATING = 0x0100
} Snake_State;

/**
 * Defines the current direction the snake is moving
 */
typedef enum {
        SNAKE_LEFT = 1,
        SNAKE_RIGHT = 2,
        SNAKE_UP = 3,
        SNAKE_DOWN = 4,
        SNAKE_NONE = 5
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
        int dir_consumed;
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
        SDL_Surface *surf;
        SDL_Rect bounds;
};
