#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>

    #include "lib/list/list.h"

#include "sprite.h"

#include "lib/gfx.h"
#include "palette.h"

typedef struct wind_t {
  int dir;
  int str;
} WIND;

WIND *MASTER_WIND;

void weather_init(void)
{
  MASTER_WIND = malloc(sizeof(WIND));
}

void wind_set(int direction)
{
   MASTER_WIND->dir = direction;
}

int wind_get(void)
{
  return MASTER_WIND->dir;
}

void wind_gfx(PANEL *pan)
{
  WINDOW *win = panel_window(pan);
  wadd_wch(win, &COMPASS[MASTER_WIND->dir]);
}

