#include "Snake.h"

struct Scene *scene;
struct Snake *snake;

void Snake_SetScene(struct Scene **ascene)
{
        scene = *ascene;
        int w = scene->w;
        int h = scene->h;
}

/**
 * Initializes a snake body part
 */
void Snake_InitBody(Body **out_body, SDL_Rect *rect, Body *prev)
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
struct Snake* Snake_Create(SDL_Renderer *renderer)
{
        snake = malloc(sizeof(struct Snake));

        SDL_Rect r = SDL_CreateRect(10, 10, 156, 156);

        snake->size = 1;
        Snake_InitBody(&snake->body, &r, NULL);
        snake->surf = SDL_CreateRGBSurface(0, 10, 10, 32,
                                           0xFF000000,
                                           0x00FF0000,
                                           0x0000FF00,
                                           0x000000FF);
        SDL_FillRect(snake->surf, NULL, 0x00000000);

        r.h = 8; r.w = 8;
        r.x = 2; r.y = 2;
        SDL_FillRect(snake->surf, &r, 0x66666677);

        r.h = 8; r.w = 8;
        r.x = 0; r.y = 0;
        SDL_FillRect(snake->surf, &r, 0x666666FF);

        return snake;
}

/**
 * Gets the tail of the snake by traversing its body
 */
Body * Snake_GetTail(Body *body)
{
        Body *abody = body;
        while (abody->next != NULL) {
                abody = abody->next;
        }

        return abody;
}

/**
 * Destroys the tail and sets the NEXT pointer to NULL
 */
void Snake_PopTail(Snake *snake)
{
        Body *abody = Snake_GetTail(snake->body);

        if (abody->prev != NULL){
                abody = abody->prev;
                free(abody->next);
                abody->next = NULL;
        } else {
                free(abody);
                abody = NULL;
        }

        snake->size--;
}

/**
 * Gets the rectangle represented at a specific point of the body
 * or the most recently found piece if outside the bounds of the tail...
 */
SDL_Rect *Snake_BodyAt(Snake *snake, int pos)
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

/**
 * Checks to ensure the snake does not leave the bounds of the game
 * and move freely about the cabin
 */
void Snake_TranslateBody(SDL_Rect * body, Snake_Direction dir)
{
        int w, h;
        w = scene->w;
        h = scene->h;

        if (body->x >= scene->w && dir == SNAKE_RIGHT)
                body->x = (body->x - scene->w);

        if (body->x < 0 && dir == SNAKE_LEFT)
                body->x = scene->w + body->x;

        if (body->y >= scene->h && dir == SNAKE_DOWN)
                body->y = body->y - scene->h;

        if (body->y < 0 && dir == SNAKE_UP)
                body->y = scene->h + body->y;
}

/**
 * Gets the next move made by the snake
 */
void Snake_MoveNext(Snake_Direction dir, SDL_Rect *out_rect)
{
        int x = 0;
        int y = 0;
        switch(dir) {
        case SNAKE_LEFT:
                x -= snake->increment;
                break;
        case SNAKE_UP:
                y -= snake->increment;
                break;
        case SNAKE_RIGHT:
                x += snake->increment;
                break;
        case SNAKE_DOWN:
                y += snake->increment;
                break;
        }

        *out_rect = snake->body->value;
        out_rect->x += x;
        out_rect->y += y;

        Snake_TranslateBody(out_rect, dir);
}


/**
 * Moves the head of the snake forward the incr amount based on direction
 */
void Snake_Move(Body *body, Snake_Direction dir, int incr)
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

        Snake_TranslateBody(&(body->value), dir);
}

/**
 * Handles input for the snake
 */
void Snake_HandleInput(Snake_Direction dir)
{
        Body *body = snake->body->next;

        body = Snake_GetTail(snake->body);
        while (body != NULL) {
#ifdef DEBUG
                printf("(%d,%d),", body->value.x, body->value.y);
#endif

                if (body->prev != NULL) {
                        body->value.x = body->prev->value.x;
                        body->value.y = body->prev->value.y;
                } else {
                        // we really only move the head forward
                        // Everything else is just copied to the location of
                        // the piece before it
                        Snake_Move(body, dir, snake->increment);
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
void Snake_Nibble(Snake_Direction dir)
{
        Body *body = Snake_GetTail(snake->body);
        int x = body->value.x;
        int y = body->value.y;
        Snake_HandleInput(dir);
        SDL_Rect r = SDL_CreateRect(10, 10, x, y);
        Snake_InitBody(&body->next, &r, body);
        snake->size++;
}

/**
 * frees all memory allocated to the snake
 */
int Snake_Free()
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
int Snake_IntersectFood(SDL_Rect *head, Food *food)
{
        SDL_Rect rect;
        rect.x = food->x;
        rect.y = food->y;
        rect.h = 10;
        rect.w = 10;

        if (SDL_HasIntersection(head, &rect) == SDL_TRUE) {
                return 0;
        }

        return -1;
}

/**
 * returns 0 if the snake collides with itself, otherwise -1
 */
int Snake_IntersectSelf(SDL_Rect *next_move)
{
        Body *abody = snake->body->next;

        while (abody != NULL) {
                if (SDL_HasIntersection(next_move, &abody->value) == SDL_TRUE) {
                        return 0;
                }
                abody = abody->next;
        }

        return -1;
}
