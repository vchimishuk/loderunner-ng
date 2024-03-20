#include <stdbool.h>
#include "texture.h"
#include "animation.h"
#include "exit.h"
#include "xmalloc.h"
#include "tile.h"

static struct sprite **sprites_init(int size)
{
    return xmalloc(sizeof(struct sprite) * size);
}

static struct sprite *runner_sprite_init(int n)
{
    int r = n / 9;
    int c = n % 9;

    return animation_sprite_init(TEXTURE_RUNNER,
        c * TILE_MAP_WIDTH, r * TILE_MAP_HEIGHT,
        TILE_MAP_WIDTH, TILE_MAP_HEIGHT);
}

static struct sprite *hole_sprite_init(int n, bool tall)
{
    int r = n / 9;
    int c = n % 9;
    int t = tall ? 2 : 1;

    return animation_sprite_init(TEXTURE_HOLE,
        c * TILE_MAP_WIDTH, r * TILE_MAP_HEIGHT * t,
        TILE_MAP_WIDTH, TILE_MAP_HEIGHT * t);
}

struct sprite *animation_sprite_init(enum texture tx, int x, int y,
    int w, int h)
{
    struct sprite *s = xmalloc(sizeof(struct sprite));
    s->texture = texture_get(tx);
    s->x = x;
    s->y = y;
    s->w = w;
    s->h = h;

    return s;
}

// TODO: Check and make sure every animation is freed.
struct animation *animation_init(enum animation_t t)
{
    struct animation *a = xmalloc(sizeof(struct animation));

    switch (t) {
    case ANIMATION_BRICK:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_BRICK, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_CLIMB_LEFT:
        a->sprites = sprites_init(6);
        a->sprites[0] = runner_sprite_init(21);
        a->sprites[1] = runner_sprite_init(22);
        a->sprites[2] = runner_sprite_init(22);
        a->sprites[3] = runner_sprite_init(23);
        a->sprites[4] = runner_sprite_init(23);
        a->sprites[5] = NULL;
        break;
    case ANIMATION_CLIMB_RIGHT:
        a->sprites = sprites_init(6);
        a->sprites[0] = runner_sprite_init(18);
        a->sprites[1] = runner_sprite_init(19);
        a->sprites[2] = runner_sprite_init(19);
        a->sprites[3] = runner_sprite_init(20);
        a->sprites[4] = runner_sprite_init(20);
        a->sprites[5] = NULL;
        break;
    case ANIMATION_DIG_LEFT:
        // Repeat dig animation as many sprites as hole digging animation
        // lasts so they have the same duration.
        a->sprites = sprites_init(12);
        a->sprites[0] = runner_sprite_init(25);
        a->sprites[1] = runner_sprite_init(25);
        a->sprites[2] = runner_sprite_init(25);
        a->sprites[3] = runner_sprite_init(25);
        a->sprites[4] = runner_sprite_init(25);
        a->sprites[5] = runner_sprite_init(25);
        a->sprites[6] = runner_sprite_init(25);
        a->sprites[7] = runner_sprite_init(25);
        a->sprites[8] = runner_sprite_init(25);
        a->sprites[9] = runner_sprite_init(25);
        a->sprites[10] = runner_sprite_init(25);
        a->sprites[11] = NULL;
        break;
    case ANIMATION_DIG_RIGHT:
        // Repeat dig animation as many sprites as hole digging animation
        // lasts so they have the same duration.
        a->sprites = sprites_init(12);
        a->sprites[0] = runner_sprite_init(24);
        a->sprites[1] = runner_sprite_init(24);
        a->sprites[2] = runner_sprite_init(24);
        a->sprites[3] = runner_sprite_init(24);
        a->sprites[4] = runner_sprite_init(24);
        a->sprites[5] = runner_sprite_init(24);
        a->sprites[6] = runner_sprite_init(24);
        a->sprites[7] = runner_sprite_init(24);
        a->sprites[8] = runner_sprite_init(24);
        a->sprites[9] = runner_sprite_init(24);
        a->sprites[10] = runner_sprite_init(24);
        a->sprites[11] = NULL;
        break;
    case ANIMATION_FALL_LEFT:
        a->sprites = sprites_init(2);
        a->sprites[0] = runner_sprite_init(26);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_FALL_RIGHT:
        a->sprites = sprites_init(2);
        a->sprites[0] = runner_sprite_init(8);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_GROUND:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_GROUND, 0, 0,
            TILE_GROUND_WIDTH, TILE_GROUND_HEIGHT);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_HOLE_FILL:
        a->sprites = sprites_init(187);
        for (int i = 0; i < 166; i++) {
            a->sprites[i] = hole_sprite_init(26, false);
        }
        for (int i = 166; i < 174; i++) {
            a->sprites[i] = hole_sprite_init(17, false);
        }
        for (int i = 174; i < 182; i++) {
            a->sprites[i] = hole_sprite_init(8, false);
        }
        // TODO: Why do we need 4-th sprite???
        for (int i = 182; i < 186; i++) {
            a->sprites[i] = hole_sprite_init(35, false);
        }
        a->sprites[186] = NULL;
        break;
    case ANIMATION_HOLE_LEFT:
        a->sprites = sprites_init(12);
        a->sprites[0] = hole_sprite_init(0, true);
        a->sprites[1] = hole_sprite_init(1, true);
        a->sprites[2] = hole_sprite_init(2, true);
        a->sprites[3] = hole_sprite_init(2, true);
        a->sprites[4] = hole_sprite_init(3, true);
        a->sprites[5] = hole_sprite_init(4, true);
        a->sprites[6] = hole_sprite_init(4, true);
        a->sprites[7] = hole_sprite_init(5, true);
        a->sprites[8] = hole_sprite_init(6, true);
        a->sprites[9] = hole_sprite_init(6, true);
        a->sprites[10] = hole_sprite_init(7, true);
        a->sprites[11] = NULL;
        break;
    case ANIMATION_HOLE_RIGHT:
        a->sprites = sprites_init(12);
        a->sprites[0] = hole_sprite_init(9, true);
        a->sprites[1] = hole_sprite_init(10, true);
        a->sprites[2] = hole_sprite_init(11, true);
        a->sprites[3] = hole_sprite_init(11, true);
        a->sprites[4] = hole_sprite_init(12, true);
        a->sprites[5] = hole_sprite_init(13, true);
        a->sprites[6] = hole_sprite_init(13, true);
        a->sprites[7] = hole_sprite_init(14, true);
        a->sprites[8] = hole_sprite_init(15, true);
        a->sprites[9] = hole_sprite_init(15, true);
        a->sprites[10] = hole_sprite_init(16, true);
        a->sprites[11] = NULL;
        break;
    case ANIMATION_LADDER:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_LADDER, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_ROPE:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_ROPE, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_LEFT:
        a->sprites = sprites_init(7);
        a->sprites[0] = runner_sprite_init(3);
        a->sprites[1] = runner_sprite_init(3);
        a->sprites[2] = runner_sprite_init(4);
        a->sprites[3] = runner_sprite_init(4);
        a->sprites[4] = runner_sprite_init(5);
        a->sprites[5] = runner_sprite_init(5);
        a->sprites[6] = NULL;
        break;
    case ANIMATION_RIGHT:
        a->sprites = sprites_init(7);
        a->sprites[0] = runner_sprite_init(0);
        a->sprites[1] = runner_sprite_init(0);
        a->sprites[2] = runner_sprite_init(1);
        a->sprites[3] = runner_sprite_init(1);
        a->sprites[4] = runner_sprite_init(2);
        a->sprites[5] = runner_sprite_init(2);
        a->sprites[6] = NULL;
        break;
    case ANIMATION_UPDOWN:
        a->sprites = sprites_init(3);
        a->sprites[0] = runner_sprite_init(6);
        a->sprites[1] = runner_sprite_init(7);
        a->sprites[2] = NULL;
        break;
    case ANIMATION_SOLID:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_SOLID, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT);
        a->sprites[1] = NULL;
        break;
    default:
        die("illegal state");
    }

    a->cur = a->sprites;

    return a;
}

/*
 * Switch to the next animation sprite.
 * Returns true it end of the animation has been reached and moved back
 * to the first sprite.
 */
bool animation_tick(struct animation *a)
{
    bool replay = false;
    a->cur++;
    if (*(a->cur) == NULL) {
        replay = true;
        animation_reset(a);
    }

    return replay;
}

/*
 * Reset animation to the first sprite.
 */
void animation_reset(struct animation *a)
{
    a->cur = a->sprites;
}
