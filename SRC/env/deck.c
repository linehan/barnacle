// vim:fdm=marker
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#include "../gfx/gfx.h"
#include "../cmd/control.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../geo/terrain.h"
#include "../geo/weather.h"
#include "../pan/instruments.h"
#include "../pan/test.h"

#include "../lib/llist/list.h"
#include "../lib/bitwise.h"
#include "../lib/ufo.h"

#include "../itm/itm.h"
#include "../mob/boat.h"
#include "../gfx/gfx_deck.h"


WINDOW *DECKW;
PANEL  *DECKP;

PANEL *deckp;

struct deck_gfx *mynao;

void init_deck(void)
{
        DECKW = newwin(30, 75, (LINES-30), ((COLS-75)/2));
        DECKP = new_panel(DECKW);

        int i, j;

        for (i=0; i<N_DECKPARTS; i++) {
                for (j=0; j<nao[i].h; j++) {
                        if (bar[i] != NULL) {
                                build_gpkg_cbar(&gfx[i][j], &bar[i][j]);
                                continue;
                        }
                        build_gpkg(&gfx[i][j]);
                }
        }
};

void draw_deck(void)
{
        mynao = malloc(sizeof(struct deck_gfx));

        int i, j; 
        for (i=0; i<N_DECKPARTS; i++) {
                WINDOW *new = newwin(nao[i].h, nao[i].w, nao[i].y0, nao[i].x0);
                mynao->P[i] = new_panel(new);
                for (j=0; j<nao[i].h; j++) {
                        mvwadd_wchstr(new, gfx[i][j].ofs[0], gfx[i][j].ofs[1], gfx[i][j].cch[0]);
                }
        }
}

void pop_deck(void)
{
        int i = N_DECKPARTS;
        while (i-->0) { top_panel(mynao->P[i]); };
}
