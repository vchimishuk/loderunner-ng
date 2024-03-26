#include <stdbool.h>
#include "keyhole.h"

static bool valid(int x, int y)
{
    return x >= 0 && x < KH_SCREEN_WIDTH
        && y >= 0 && y < KH_SCREEN_HEIGHT;
}

static void set_pixel(bool scr[KH_SCREEN_WIDTH][KH_SCREEN_HEIGHT],
    int xc, int yc, int x, int y)
{
    if (valid(xc + x, yc + y)) {
        scr[xc + x][yc + y] = true;
    }
    if (valid(xc - x, yc + y)) {
        scr[xc - x][yc + y] = true;
    }
    if (valid(xc + x, yc - y)) {
        scr[xc + x][yc - y] = true;
    }
    if (valid(xc - x, yc - y)) {
        scr[xc - x][yc - y] = true;
    }
    if (valid(xc + y, yc + x)) {
        scr[xc + y][yc + x] = true;
    }
    if (valid(xc - y, yc + x)) {
        scr[xc - y][yc + x] = true;
    }
    if (valid(xc + y, yc - x)) {
        scr[xc + y][yc - x] = true;
    }
    if (valid(xc - y, yc - x)) {
        scr[xc - y][yc - x] = true;
    }
}

static void floodfill(bool scr[KH_SCREEN_WIDTH][KH_SCREEN_HEIGHT], int x, int y)
{
    if (x < 0 || x >= KH_SCREEN_WIDTH || y < 0 || y >= KH_SCREEN_HEIGHT) {
        return;
    }
    if (scr[x][y]) {
        return;
    }

    scr[x][y] = true;
    floodfill(scr, x, y - 1);
    floodfill(scr, x + 1, y);
    floodfill(scr, x, y + 1);
    floodfill(scr, x - 1, y);
}

static void render_pixel(SDL_Renderer *renderer, int x, int y)
{
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = KH_PIXEL;
    r.h = KH_PIXEL;
    // TODO: Handle error.
    SDL_RenderFillRect(renderer, &r);
}

/*
 * Draw keyhole of radius r (in KH_PIXEL) at the center of the screen.
 * It uses Bresenham’s circle drawing algorithm to draw the circle.
 * See: https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm
 */
void keyhole_render(SDL_Renderer *renderer, int r)
{
    bool screen[KH_SCREEN_WIDTH][KH_SCREEN_HEIGHT];
    for (int i = 0; i < KH_SCREEN_WIDTH; i++) {
        for (int j = 0; j < KH_SCREEN_HEIGHT; j++) {
            screen[i][j] = false;
        }
    }

    int xc = KH_SCREEN_WIDTH / 2;
    int yc = KH_SCREEN_HEIGHT / 2;
    int x = 0;
    int y = r;
    int d = 3 - (2 * r);

    // Mark circle pixels as visible.
    do {
        set_pixel(screen, xc, yc, x, y);

        x += 1;
        if (d < 0) {
            d = d + (4 * x) + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y -= 1;
        }
    } while (x <= y);

    // Fill circle inside with visible pixels.
    floodfill(screen, xc, yc);

    // And render pseudo-pixels.
    for (int i = 0; i < KH_SCREEN_WIDTH; i++) {
        for (int j = 0; j < KH_SCREEN_HEIGHT; j++) {
            if (!screen[i][j]) {
                render_pixel(renderer, i * KH_PIXEL, j * KH_PIXEL);
            }
        }
    }
}
