#pragma once
#ifndef __NOTIFY_GLOSS_H
#define __NOTIFY_GLOSS_H
#include "../com/barnacle.h"
#include "../gfx/palette.h"
#include "effects.h"

typedef struct gloss_t GLOSS;

/* Color schemes */
#define __GOLD  PUR_METALYELLOW, PUR_YELLOW
#define __SILV  PUR_WHITE, PUR_SKY
#define __WARN  PUR_WHITE, PUR_RED
#define __KILL  PUR_WHITE, PUR_RED
#define __HULK  PUR_SKY, PUR_LGREEN
#define __CERU  PUR_SKY, PUR_CERULEAN


struct gloss_t *new_gloss(WINDOW *win, const wchar_t *wcs, short co, short hi);
void            del_gloss(struct gloss_t *g);
bool                gloss(struct gloss_t *g);


short gloss_co(struct gloss_t *gloss);
short gloss_hi(struct gloss_t *gloss);


#endif


