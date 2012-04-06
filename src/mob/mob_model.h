#pragma once
#ifndef __MOB_H
#define __MOB_H

#include "../gfx/gfx.h"
#include "../map/map.h"
#include "../lib/ufo.h"
#include "../map/snake.h"


struct mob_t {
        PANEL *pan;
        struct ufo_t ufo;
        struct path_t path;
};


void set_mob(struct mob_t *mob, struct map_t *map, int h, int w, int y0, int x0);
void move_mob(struct mob_t *mob, int dir);
int mob_control(struct mob_t *mob, int order, int val);
struct mob_t *get_mob(const char *name);

#endif
