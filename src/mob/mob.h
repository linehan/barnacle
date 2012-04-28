#pragma once
#ifndef __MOB_H
#define __MOB_H

#include "../gfx/gfx.h"
#include "../map/map.h"
#include "../lib/ufo.h"
#include "../map/snake.h"
#include "../ai/a_star.h"


struct map_t;


struct mob_t {
        PANEL *pan;
        WINDOW *win;
        struct animation *animate;
        struct ufo_t ufo;
        struct path_t *path;
        struct astar_t *astar;
        uint32_t signal;
        bool active;
        uint32_t name; 
};


static inline PANEL *mob_pan(struct mob_t *mob)
{
        return (mob->pan);
}
static inline WINDOW *mob_win(struct mob_t *mob)
{
        return panel_window(mob->pan);
}

static inline void mob_pos(struct mob_t *mob, uint32_t *y, uint32_t *x)
{
        *y = ufo_y(mob, ufo);
        *x = ufo_x(mob, ufo);
}


void mob_cfg(struct mob_t *mob, struct map_t *map, int h, int w, int y0, int x0);
void set_mob(struct mob_t *mob, bool onoff);
void mob_move(struct mob_t *mob, int dir);
void mob_path(struct mob_t *mob);
void mob_animate(struct mob_t *mob);
void mob_set_signal(struct mob_t *mob, int verb, int dir);

#endif

