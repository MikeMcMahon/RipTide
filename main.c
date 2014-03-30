#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include "SDL_Util.h"

#include "Game.h"
#include "Food.h"
#include "Snake.h"

#define DEFAULT_WIDTH 250
#define DEFAULT_HEIGHT 250

Scene *scene;
Snake_Direction snake_moves[2] = { SNAKE_NONE, SNAKE_NONE };

void Move_Add(Snake_Direction dir)
{
        if (snake_moves[0] == SNAKE_NONE)
                snake_moves[0] = dir;
        else
                snake_moves[1] = dir;
}

Snake_Direction Move_Pop()
{
        Snake_Direction d = snake_moves[0];
        if (snake_moves[1] == SNAKE_NONE)
                snake_moves[0] = d;
        else
                snake_moves[0] = snake_moves[1];

        snake_moves[1] = SNAKE_NONE;
        return d;
}

Snake_Direction * Move_Peek()
{
        return &snake_moves[0];
}

int Scene_Create(Scene **outscene, int w, int h)
{
        struct _Scene * ascene = malloc(sizeof(struct _Scene));

        ascene->h = h;
        ascene->w = w;
        ascene->state = GAME_START_SCREEN;
        ascene->window = SDL_CreateWindow(
                                  "Snake",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  w,
                                  h,
                                  SDL_WINDOW_SHOWN);
        if (ascene->window == NULL)
                return -1;

        ascene->renderer = SDL_CreateRenderer(
                   ascene->window,
                   -1,
                   SDL_RENDERER_ACCELERATED);

        if (ascene->renderer == NULL)
                return -2;

        ascene->window_surface = SDL_GetWindowSurface(ascene->window);

        if (ascene->window_surface == NULL)
                return -3;

        ascene->texture = SDL_CreateTextureFromSurface(
                                               ascene->renderer,
                                               ascene->window_surface);
        if (ascene->texture == NULL)
                return -4;

        *outscene = ascene;
        return 0;
};

/******************** SDL ******************/

void update(Snake *, Food *);
void render(Snake *, Food *);
int processInput(Snake *, SDL_Event *);
void cleanup(Snake *, Food *);

int main (int argc, char *args[])
{
        SDL_bool quit = SDL_FALSE;
        SDL_Event e;
        SDL_Surface *font_surface;
        TTF_Font *font;
        Food *food = Food_Create();

        /** TIME KEEPING **/
        unsigned int previous = SDL_GetTicks();
        unsigned int current = 0;
        unsigned int lag = 0;
        unsigned int elapsed = 0;
        /** TIME KEEPING **/

        Snake * snake;
        float snake_fps = 16;
        float snake_movement = 10;
        float snake_ms_per_update = (1000 / snake_fps);

        srand(time(NULL));

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
                return 1;

        if (Scene_Create(&scene, DEFAULT_WIDTH, DEFAULT_HEIGHT) != 0)
                return 1;

        snake = Snake_Create(scene->renderer);
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
        font_surface = TTF_RenderText_Blended(font, "Snake", fg);

        SDL_Rect font_location;
        font_location.w = font_surface->w;
        font_location.h = font_surface->h;
        font_location.x = (DEFAULT_WIDTH / 2) - (font_location.w / 2);
        font_location.y = 0;

        Food_GenerateLoc(food);
        //Food_Move(food, snake);
        Move_Add(SNAKE_LEFT);

        while (!quit) {
                current = SDL_GetTicks();
                elapsed = current - previous;
                previous = current;
                lag += elapsed;

                SDL_PollEvent(&e);
                if (processInput(snake, &e) != 0)
                        quit = SDL_TRUE;

                while (lag >= snake_ms_per_update) {
                        update(snake, food);
                        lag -= snake_ms_per_update;
                }

                SDL_RenderClear(scene->renderer);
                render(snake, food);
                SDL_BlitSurface(
                                font_surface,
                                NULL,
                                scene->window_surface,
                                &font_location);
                SDL_UpdateTexture(scene->texture, NULL,
                                  scene->window_surface->pixels,
                                  scene->window_surface->pitch);
                SDL_RenderCopy(scene->renderer, scene->texture, NULL, NULL);

                SDL_RenderPresent(scene->renderer);
        }

        cleanup(snake, food);
        return 0;
}

void cleanup(Snake *snake, Food *food)
{

        Food_Free(food);
        Snake_Free(snake);

        TTF_Quit();

        SDL_FreeSurface(scene->window_surface);
        SDL_DestroyTexture(scene->texture);
        SDL_DestroyRenderer(scene->renderer);
        SDL_DestroyWindow(scene->window);
        SDL_Quit();

        free(scene);
}

void render(Snake *snake, Food *food) {
        SDL_Rect bounds;
        SDL_RenderClear(scene->renderer);

        SDL_FillRect(scene->window_surface, NULL, 0xFFBECFAE);

        for (int i = 1; i <= snake->size; i++) {
                bounds = *Snake_BodyAt(snake, i);
                SDL_BlitSurface(snake->surf, NULL, scene->window_surface, &bounds);
        }

        SDL_BlitSurface(food->surf, NULL, scene->window_surface, &food->bounds);
}

void update(Snake *snake, Food *food) {
        SDL_Rect next_move;
        SDL_Rect new_body;

        Snake_Direction dir = Move_Pop();

        if (snake->state == SNAKE_EATING) {
                snake->state = SNAKE_ALIVE;
                Food_GenerateLoc(food);
                //Food_Move(food, snake);
                Snake_Nibble(snake, dir);
                return;
        }
        if (snake->state == SNAKE_ALIVE) {
                Snake_MoveNext(snake, dir, &next_move);
                if (Snake_IntersectFood(&next_move, food) == 0) {
                        snake->state = SNAKE_EATING;
                }
                if (Snake_IntersectSelf(snake, &next_move) == 0) {
                        snake->state = SNAKE_DYING;
                } else {
                        Snake_HandleInput(snake, dir);
                }
        } else if (snake->state == SNAKE_DYING) {
                if (snake->size > 0) {
                        Snake_PopTail(snake);
                } else if (snake->size == 0) {
                        new_body = SDL_CreateRect(10, 10, 156, 156);
                        Snake_InitBody(&snake->body, &new_body, NULL);
                        snake->state = SNAKE_ALIVE;
                        snake->size++;
                        dir = SNAKE_LEFT;
                }
        }
}

/**
 * Handles the input from the game and returns -1 if should quit
 * otherwise a zero that input was handled per usual
 */
int processInput(Snake *snake, SDL_Event *e)
{
        Snake_Direction new_dir = SNAKE_NONE;

        if (e->type == SDL_QUIT)
                return -1;

        if (e->type == SDL_KEYDOWN && e->key.state == SDL_PRESSED) {
                if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE] != 0)
                        return -1;

                switch (e->key.keysym.sym) {
                case SDLK_LEFT:
                        new_dir = SNAKE_LEFT;
                        break;
                case SDLK_UP:
                        new_dir = SNAKE_UP;
                        break;
                case SDLK_RIGHT:
                        new_dir = SNAKE_RIGHT;
                        break;
                case SDLK_DOWN:
                        new_dir = SNAKE_DOWN;
                        break;
                default:
                        new_dir = SNAKE_NONE;
                }

                if (Snake_MoveValid(snake, *Move_Peek(), new_dir) == 0)
                        Move_Add(new_dir);
        }

        return 0;
}
