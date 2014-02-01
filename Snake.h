#pragma once

#include "SDL2/SDL.h"
#include "SDL_Util.h"
#include "Game.h"

void Snake_SetScene(Scene **);

struct _Snake * Snake_Create();
void Snake_InitBody(Body **out_body, SDL_Rect *rect, Body *prev);
int Snake_Free(Snake *);

SDL_Rect *Snake_BodyAt(Snake *snake, int pos);
Body * Snake_GetTail(Body *body);
void Snake_PopTail(Snake *);

void Snake_HandleInput(Snake *, Snake_Direction);
int Snake_MoveValid(Snake *, Snake_Direction, Snake_Direction);
void Snake_Move(Body *, Snake_Direction, int);
void Snake_MoveNext(Snake *, Snake_Direction, SDL_Rect *);
void Snake_TranslateBody(SDL_Rect *, Snake_Direction);


void Snake_Nibble(Snake *, Snake_Direction dir);
int Snake_IntersectFood(SDL_Rect *head, Food *food);
int Snake_IntersectSelf(Snake *, SDL_Rect *next_move);
