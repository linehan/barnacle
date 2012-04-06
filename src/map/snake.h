#pragma once
#ifndef __SNAKE_PATH
#define __SNAKE_PATH
#include <stdint.h>
#include "../gfx/gfx.h"
#include "../lib/fifo.h"

////////////////////////////////////////////////////////////////////////////////
static const uint32_t SEG_N = 5;
static const uint32_t SEG_DEFAULT[]={2, 4, 9, 11, 13};
static const uint32_t SEG_BASELEN = 13;
////////////////////////////////////////////////////////////////////////////////

struct path_t {
        uint32_t seg[5];
        uint32_t origin;
        struct fifo_t fifo;
};

struct path_t *new_path(int yofs, int xofs, int len);
void init_path(struct path_t *path, int yofs, int xofs, int len);
void path_push(struct path_t *path, int y, int x);
void path_pop(struct path_t *path);

void draw_path(WINDOW *win, struct path_t *path);

#endif
