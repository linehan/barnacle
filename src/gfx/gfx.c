/* gfx.c */

#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "gfx.h"

#include "../lib/stoc/stoc.h"
#include "../map/map.h"
#include "../test/test.h"
#include "../map/terrain.h"


attr_t saved_attr;
short saved_pair;
#define SAVEWIN(win) wattr_get((win), &saved_attr, &saved_pair, NULL)
#define RESTORE(win) wattr_set((win), saved_attr, saved_pair, NULL)


/*
 * Paint a wide character string and its rendition on the screen
 */
void mvwp(WINDOW *win, int y, int x, wchar_t *wch, short pair, attr_t attr)
{
        cchar_t cch; 

        setcchar(&cch, wch, attr, pair, NULL);
        mvwadd_wch(win, y, x, &cch);
}


/*
 * Paint a wide character string and its rendition on the screen
 */
void mvwsnpaint(WINDOW *win, int y, int x, wchar_t *wcs, short pair, int n)
{
        SAVEWIN(win);
        wcolor_set(win, pair, NULL);
        mvwaddnwstr(win, y, x, wcs, n);
        RESTORE(win);
}




void wcch(WINDOW *win, const wchar_t *wch, attr_t attr, short pair)
{
        SAVEWIN(win);
        wcolor_set(win, pair, NULL);
        if (wch && (*wch != L'\0')) {
                waddnwstr(win, wch, 1);
        }
        RESTORE(win);
}

void mvwcch(WINDOW *win, int y, int x, const wchar_t *wch, attr_t attr, short pair)
{
        SAVEWIN(win);
        wcolor_set(win, pair, NULL);
        if (wch && (*wch != L'\0')) {
                mvwaddnwstr(win, y, x, wch, 1);
        }
        RESTORE(win);
}




void geojug_start(void)
{
        /*GLOBE = malloc(sizeof(struct map_t)); // initialize the global map*/
}


void build_gpkg(struct gpkg *g)
{
        int n, i;
        n = g->n;

        while (n-->0) {
               g->len[n] = (sizeof(wchar_t))*(wcslen(g->wch[n])+1);
               g->cch[n] = malloc(g->len[n] * sizeof(cchar_t));
               for (i=0; i<(g->len[n]); i++) {
                       setcchar(&g->cch[n][i], &g->wch[n][i], 0, g->pair, NULL);
               }
        }
}


void build_gpack(struct gpack *g)
{
        setcchar(&g->cch, g->wcs, 0, g->pair, NULL);
}

cchar_t shared_cch;

cchar_t *mkcch(wchar_t *wch, attr_t attr, short co)
{
        setcchar(&shared_cch, wch, attr, co, NULL);
        return (&shared_cch);
}


void center_text(WINDOW *win, int y0, int x0, int w, char *string)
{
	int len, x, y;
	float temp;

	getyx(win, y, x);
	if (x0 != 0) x = x0;
	if (y0 != 0) y = y0;
	if (w  == 0) w = 80;

	len = strlen(string);
	temp = (w - len)/2;
	x = x0 + (int)temp;
	mvwprintw(win, y, x, "%s", string);
}


void wwrapstr(WINDOW *win, const char *string)
{
        int width = getmaxx(win);
        int ch;
        int i;
        int wrap; // The index at which to wrap the string.

        i = wrap = 0;

        while (ch = string[i++]) {
                if ((ch==' ')&&(i<width)) wrap = i;
        }
        /* If the line will fit, simply print it. */
        if (i < width) {
                waddstr(win, string);
                return;
        }
        /* Else wrap it. */
        else {
                waddnstr(win, string, wrap);
                wprintw(win, "\n");
                waddstr(win, &string[wrap]);
        }
}

short cuco(WINDOW *win)
{
        attr_t attrs;
        short color_pair;

        wattr_get(win, &attrs, &color_pair, NULL);

        return (color_pair);
}

inline short WINPAIR(WINDOW *win, int y, int x)
{
        cchar_t cch;
        wchar_t wch;
        short color;
        attr_t attr;

        getbegyx(win, y, x);           /* Get the position of src */
        mvwin_wch(win, y, x, &cch);    /* Get the cch at position in dst */

        /* Extract character renditions */
        getcchar(&cch, &wch, &attr, &color, NULL);

        return (color);
}


short bgcolor_yx(WINDOW *win, int y, int x)
{
        cchar_t cch;
        wchar_t wch;
        short pair = 0;
        short bg   = 0;
        short fg   = 0;
        attr_t attr;

        mvwin_wch(win, y, x, &cch);    /* Get the cch at position in src */

        /* Extract character renditions */
        getcchar(&cch, &wch, &attr, &pair, NULL);

        /* Extract the fg and bg color components of the pairs */
        pair_content(pair, &fg, &bg);

        return (bg);
}


void take_bkgrnd(WINDOW *dst, WINDOW *src, short pair)
{
        short src_pair, src_bg, src_fg;
        short dst_fg;
        attr_t src_attr;
        cchar_t cch;
        wchar_t wch;
        int y, x;

        getbegyx(dst, y, x);         /* Get the position of dst */
        mvwin_wch(src, y, x, &cch);  /* Get the cch at position in src */

        if (getcchar(&cch, &wch, &src_attr, &src_pair, NULL) == ERR)
                return;

        pair_content(src_pair, &src_fg, &src_bg);

        dst_fg = fgcolor(pair);          /* Use foreground of provided pair */
        init_pair(pair, dst_fg, src_bg); /* Re-init pair with src background */
}



bool is_blank(WINDOW *win, int y, int x)
{
        cchar_t cch;
        wchar_t wch;
        attr_t attr;
        short color;

        mvwin_wch(win, y, x, &cch);

        getcchar(&cch, &wch, &attr, &color, NULL);

        return (wch == L' ') ? true : false;
}

