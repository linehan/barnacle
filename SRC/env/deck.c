// vim:fdm=marker
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>

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
#include "cloud.h"

cchar_t **wavez;

int start;

WINDOW *DECKW, *WAVEW, *SKYW;
PANEL  *DECKP, *WAVEP, *SKYP;



PANEL *deckp;

cchar_t DEEPNESS[3];

struct deck_gfx *mynao;

void init_deck(void)
{
        init_cloud();
        
        setcchar(&DEEPNESS[0], L"▓", 0, D_SEA, NULL);
        setcchar(&DEEPNESS[1], L"▒", 0, D_SEA, NULL);
        setcchar(&DEEPNESS[2], L"▒", 0, GRE_SKY, NULL);

        start = 0;
        wavez = malloc(COLS * sizeof(cchar_t *));
        wavefill(wavez, COLS);

        DECKW = newwin(LINES, COLS, 0, 0);
        DECKP = new_panel(DECKW);

        WAVEW = newwin(13, COLS, LINES-13, 0);
        wbkgrnd(WAVEW, &UNDERSEA);

        mvwhline_set(WAVEW, 15, 0, &DEEPNESS[1], COLS);
        mvwhline_set(WAVEW, 16, 0, &DEEPNESS[1], COLS);
        mvwhline_set(WAVEW, 17, 0, &DEEPNESS[1], COLS);
        vrt_refresh();
        overlay(WAVEW, DECKW);
        vrt_refresh();
        WAVEP = new_panel(WAVEW);

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
        for (i=0; i<COLS; i++) {
                mvwadd_wch(WAVEW, 0, i, wavez[i]);
        }
};

void wrap_wave(char dir)
{
        const int w = COLS;

        if (dir == 'l') {
                if (start == 0) start = COLS; 
                else            start--;
        }
        if (dir == 'r') {
                if (start == COLS)      start = 0;
                else                    start++;
        }

        int i;
        werase(WAVEW);
        for (i=0; i<w; i++) {
                mvwadd_wch(WAVEW, 0, i, wavez[((i+start)%w)]);
        }
        vrt_refresh();
        pop_deck();
}
        
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
        WINDOW *win;
        overlay(PEEK(GLOBE->W), DECKW);
        /*overlay(WAVEW, DECKW);*/
        while (i-->0) {
                win = panel_window(mynao->P[i]);
                overlay(win, DECKW);
        };

        if (!panel_hidden(DECKP)) top_panel(DECKP);

        vrt_refresh();
}
