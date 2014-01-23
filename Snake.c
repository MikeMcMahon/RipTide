#include "Snake.h"
#include "SDL_Util.h"
#include <stdlib.h>

/*
void init_snake_body(Body *body, Body *prev)
{
    struct Body *body = malloc(sizeof(struct Body*));
    body->value = NULL;
    body->next = NULL;
    body->prev = prev;
}

void create_snake(SDL_Renderer *renderer, Snake *snake) {
    SDL_Rect r = SDL_CreateRect(10, 10, 100, 100);

    snake->size = 1;
    snake->body = malloc(sizeof(struct Body));
    snake->body->next = NULL;
    snake->body->prev = NULL;
    snake->body->value = r;

    snake->surf = SDL_CreateRGBSurface(0, 10, 10, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_FillRect(snake->surf, NULL, 0xAAAAAAFF);
    snake->texture = SDL_CreateTextureFromSurface(renderer, snake->surf);
}

void reset_body(Body *body)
{
    if (body->prev != NULL)
    {
        body = body->prev;
        reset_body(body);
    }
}

Body * get_snake_tail(Body *body, int len)
{
    Body *abody = body;
    for (int i = 0; i < len; i++)
    {
        if (abody->next != (struct Body*)NULL)
            abody = abody->next;
    }

    return abody;
}

Body *get_next_body(Body *body)
{
    if (body->next != (struct Body*) NULL)
        return body->next;
    return NULL;
}
SDL_Rect *get_body_at(Snake *snake, int len)
{
    Body *abody = snake->body;
    SDL_Rect *rect;

    for (int c = 0; c < len; c++) // hehe
    {
        if (abody != (struct Body*)NULL)
        {
            rect = &abody->value;
        }
        abody = get_next_body(abody);
    }

    return rect;
}

void snake_nibble(Snake *snake)
{
    Body *body = get_snake_tail(snake->body, snake->size);
    body->next = malloc(sizeof(struct Body));
    body->next->next = NULL;
    body->next->prev = body;
    body->next->value = SDL_CreateRect(10, 10, body->value.x + 10 + 2, 100);
    snake->size++;
}

*/
