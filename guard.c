#include "exit.h"
#include "guard.h"
#include "phys.h"
#include "texture.h"
#include "tile.h"
#include "xmalloc.h"

struct animation *guard_state_animation(struct guard *g, enum guard_state s)
{
    switch (s) {
    case GSTATE_CLIMB_LEFT:
        return g->climblefta;
    case GSTATE_CLIMB_OUT:
        return g->updowna;
    case GSTATE_CLIMB_RIGHT:
        return g->climbrighta;
    case GSTATE_FALL_LEFT:
        return g->falllefta;
    case GSTATE_FALL_RIGHT:
        return g->fallrighta;
    case GSTATE_LEFT:
        return g->lefta;
    case GSTATE_REBORN:
        return g->reborna;
    case GSTATE_RIGHT:
        return g->righta;
    case GSTATE_TRAP_LEFT:
        return g->traplefta;
    case GSTATE_TRAP_RIGHT:
        return g->traprighta;
    case GSTATE_UPDOWN:
        return g->updowna;
    default:
        die("illegal state");
    }
}

struct guard *guard_init()
{
    struct guard *g = xmalloc(sizeof(struct guard));
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

// TODO: Looks like it is used only by guard_init(). Remove it then.
void guard_reset(struct guard *g)
{
    // TODO: Start position?
    g->x = 0;
    g->y = 0;
    g->tx = 0;
    g->ty = 0;
    g->cura = g->lefta;
    g->state = GSTATE_LEFT;
    g->holey = -1;
    g->gold = NULL;
    g->goldholds = 0;
}
