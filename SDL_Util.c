#include "SDL_Util.h"

SDL_Rect SDL_CreateRect(int h, int w, int x, int y) {
    SDL_Rect rc;
    rc.h = h; rc.w = w;
    rc.x = x; rc.y = y;

    return rc;
}


/**
 * Takes a pointer to an array of pixels and the pixel pitch and applies a grid
 * like filter using the alpha channel, makes some huge assumptions
 * Assumes ARGB, Assumes 8888 bit color
 */
void SDL_GridFilter(Uint8 *pixels, int pitch, Uint8 alpha, int h, int w, int size)
{
        // have a 10x10 and we want to
        for (int x = size; x < w; x+=size+1) {
                for (int y = 0; y < h; y++) {
                        pixels[(y * pitch) + (x * sizeof(Uint32))] = alpha;
                        //*pixels = alpha;
                }
        }

        for (int x = 0; x < w; x++) {
                for (int y = size; y < h; y+= size+1) {
                        pixels[(y * pitch) + (x * sizeof(Uint32))] = alpha;
                        //*pixels = alpha;
                }
        }
}
