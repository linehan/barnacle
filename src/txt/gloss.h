#pragma once
#ifndef __NOTIFY_GLOSS_H
#define __NOTIFY_GLOSS_H
#include "../com/arawak.h"
#include "../gfx/palette.h"
#include "effects.h"

/* 
 * DATA STRUCTURE
 * The gloss object
 */
struct gloss_t {
        WINDOW *win;
        wchar_t *wcs;
        size_t len;
        short co;
        short hi;
        short hue[2];
        void (*put[NSTYLES])(void *self, int loop);
};

typedef struct gloss_t GLOSS;

struct gloss_t *new_gloss(WINDOW *win, wchar_t *wcs, short co, short hi);
struct gloss_t *str_gloss(WINDOW *win,    char *str, short co, short hi);
void del_gloss(struct gloss_t *g);

bool gloss(struct gloss_t *g);


#endif


