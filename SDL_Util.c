#include "SDL_Util.h"

SDL_Rect SDL_CreateRect(int h, int w, int x, int y) {
    SDL_Rect rc;
    rc.h = h; rc.w = w;
    rc.x = x; rc.y = y;

    return rc;
}
