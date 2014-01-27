#pragma once

#include "SDL2/SDL.h"
#include "SDL_Util.h"
#include "Game.h"

void Snake_SetScene(Scene **);

struct _Snake * Snake_Create(SDL_Renderer *renderer);
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
