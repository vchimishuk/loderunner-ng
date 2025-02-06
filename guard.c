#include "exit.h"
#include "guard.h"
#include "phys.h"
#include "texture.h"
#include "xmalloc.h"

struct animation *guard_state_animation(struct guard *g, enum guard_state s)
{
    struct animation *a;

    switch (s) {
    case GSTATE_CLIMB_LEFT:
        a = g->climblefta;
        break;
    case GSTATE_CLIMB_OUT:
        a = g->updowna;
        break;
    case GSTATE_CLIMB_RIGHT:
        a = g->climbrighta;
        break;
    case GSTATE_FALL_LEFT:
        a = g->falllefta;
        break;
    case GSTATE_FALL_RIGHT:
        a = g->fallrighta;
        break;
    case GSTATE_LEFT:
        a = g->lefta;
        break;
    case GSTATE_REBORN:
        a = g->reborna;
        break;
    case GSTATE_RIGHT:
        a = g->righta;
        break;
    case GSTATE_TRAP_LEFT:
        a = g->traplefta;
        break;
    case GSTATE_TRAP_RIGHT:
        a = g->traprighta;
        break;
    case GSTATE_UPDOWN:
        a = g->updowna;
        break;
    default:
        die("illegal state");
    }

    animation_reset(a);

    return a;
}

struct guard *guard_init(int x, int y)
{
    struct guard *g = xmalloc(sizeof(struct guard));
    g->sx = x;
    g->sy = y;
    g->x = 0;
    g->y = 0;
    g->tx = 0;
    g->ty = 0;
    g->lefta = animation_init(ANIMATION_GUARD_LEFT);
    g->righta = animation_init(ANIMATION_GUARD_RIGHT);
    g->updowna = animation_init(ANIMATION_GUARD_UPDOWN);
    g->climblefta = animation_init(ANIMATION_GUARD_CLIMB_LEFT);
    g->climbrighta = animation_init(ANIMATION_GUARD_CLIMB_RIGHT);
    g->falllefta = animation_init(ANIMATION_GUARD_FALL_LEFT);
    g->fallrighta = animation_init(ANIMATION_GUARD_FALL_RIGHT);
    g->traplefta = animation_init(ANIMATION_GUARD_TRAP_LEFT);
    g->traprighta = animation_init(ANIMATION_GUARD_TRAP_RIGHT);
    g->reborna = animation_init(ANIMATION_GUARD_REBORN);

    guard_reset(g);

    return g;
}

void guard_destroy(struct guard *g)
{
    animation_destroy(g->lefta);
    animation_destroy(g->righta);
    animation_destroy(g->updowna);
    animation_destroy(g->climblefta);
    animation_destroy(g->climbrighta);
    animation_destroy(g->falllefta);
    animation_destroy(g->fallrighta);
    animation_destroy(g->traplefta);
    animation_destroy(g->traprighta);
    animation_destroy(g->reborna);
    free(g);
}

void guard_reset(struct guard *g)
{
    g->x = g->sx;
    g->y = g->sy;
    g->tx = 0;
    g->ty = 0;
    g->cura = guard_state_animation(g, GSTATE_LEFT);
    g->state = GSTATE_LEFT;
    g->holey = -1;
    g->gold = NULL;
    g->goldholds = 0;
}
