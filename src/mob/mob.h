#pragma once
#ifndef __MOB_H
#define __MOB_H

#include "../gfx/gfx.h"
#include "../map/map.h"
#include "../lib/rec.h"
#include "../map/snake.h"
#include "../ai/a_star.h"

bool light_torch;

struct map_t;
struct noun_t *noun_forw_ref;
enum mob_facing { MOB_NORTH, MOB_SOUTH, MOB_EAST, MOB_WEST };


struct mob_t {
        bool active;
        struct pos_t *pos;
        struct astar_t *astar;
        struct ani_t *animate;

        uint32_t map_id;
        uint32_t noun_id;

        PANEL  *pan;
        WINDOW *win;

        bool hit_testing_enabled;
        void (*step)(void *self, int dir);
        void (*setyx)(void *self, int y, int x);
        void (*hit)(void *self);
};


static inline PANEL *mob_pan(struct mob_t *mob)
{
        return (mob->pan);
}
static inline WINDOW *mob_win(struct mob_t *mob)
{
        return panel_window(mob->pan);
}
static inline void mob_move(struct mob_t *mob, int dir)
{
        mob->step(mob, dir);
}


struct mob_t *new_mob(struct noun_t *noun, struct map_t *map, int h, int w, int y, int x);

void mob_cfg(struct mob_t *mob, struct map_t *map, int h, int w, int y0, int x0);
void set_mob(struct mob_t *mob, bool onoff);
void mob_path(struct mob_t *mob);
void mob_animate(struct mob_t *mob);
void mob_set_signal(struct mob_t *mob, int verb, int dir);
void mob_seek(struct noun_t *snoun, struct noun_t *gnoun);
void mob_fall(struct mob_t *mob);

#endif

