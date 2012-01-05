#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>

#include "palette.h"
#include "ticker.h"
#include "weather.h"

typedef struct mob_t {
  PANEL *pan;
  int      x;
  int      y;
  int      w;
  int      h;
} MOB;

/*----------------------------------------------------------------------------*/
MOB *makemob(PANEL *mypanel, int H, int W, int Y, int X)
{
  MOB *mob = malloc(sizeof(MOB));

  mob->pan = mypanel;
  mob->h = H;
  mob->w = W;
  mob->y = Y;
  mob->x = X;

  return mob;
}
/*----------------------------------------------------------------------------*/
