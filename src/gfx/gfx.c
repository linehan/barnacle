#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "gfx.h"

#include "../lib/stoc/stoc.h"
#include "../map/map.h"
#include "../test/test.h"
#include "../map/terrain.h"


/*
 * NOTE
 * wcch and mvwcch change the window's attributes; to ensure that
 * each character may have a different rendition and thus behave
 * as though we were building cchar_t's every time, the current
 * window attributes are saved and then restored before the functions
 * return.
 */
attr_t saved_attr;
short saved_pair;
#define SAVEWIN(win) wattr_get((win), &saved_attr, &saved_pair, NULL)
#define RESTORE(win) wattr_set((win), saved_attr, saved_pair, NULL)


/**
 * wcch -- write a complex character and rendition to a window
 * @win : pointer to a WINDOW
 * @wch : pointer to the wchar_t to be written
 * @attr: the desired window attributes
 * @pair: the desired color pair
 */
void wcch(WINDOW *win, const wchar_t *wch, attr_t attr, short pair)
{
        SAVEWIN(win);
        wattr_set(win, attr, pair, NULL);
        if (wch && (*wch != L'\0')) {
                waddnwstr(win, wch, 1);
        }
        RESTORE(win);
}


/**
 * mvwcch -- move the cursor and write a complex character and rendition
 * @win : pointer to a WINDOW
 * @y   : y-coordinate to write at
 * @x   : x-coordinate to write at
 * @wch : pointer to the wchar_t to be written
 * @attr: the desired window attributes
 * @pair: the desired color pair
 */
void mvwcch(WINDOW *win, int y, int x, const wchar_t *wch, attr_t attr, short pair)
{
        SAVEWIN(win);
        wattr_set(win, attr, pair, NULL);
        if (wch && (*wch != L'\0')) {
                mvwaddnwstr(win, y, x, wch, 1);
        }
        RESTORE(win);
}


/**
 * mkcch -- return pointer to a "shared" cchar_t after applying rendition to it
 * @wch : pointer to the wchar_t to contain in the cchar 
 * @attr: the desired window attributes of the cchar
 * @pair: the desired color pair of the cchar
 */
cchar_t SHARED_CCH;
cchar_t *mkcch(const wchar_t *wch, attr_t attr, short pair)
{
        setcchar(&SHARED_CCH, wch, attr, pair, NULL);
        return (&SHARED_CCH);
}


/* HELPERS AND USEFUL UTILITIES
``````````````````````````````````````````````````````````````````````````````*/
/**
 * is_blank -- return TRUE if wide-character at (y,x) is blank, else FALSE
 * @win: pointer to a window
 * @y  : y-coordinate to extract wide-character at
 * @x  : x-coordiante to extract wide-character at
 */
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




/* COLORING
``````````````````````````````````````````````````````````````````````````````*/
/**
 * bgcolor -- return the background color of a color pair */
inline short bgcolor(short pair)
{
        short fg, bg;
        pair_content(pair, &fg, &bg);
        return bg;
}

/**
 * fgcolor -- return the foreground color of a color pair */
inline short fgcolor(short pair)
{
        short fg, bg;
        pair_content(pair, &fg, &bg);
        return fg;
}

/**
 * winpair_yx -- return the color pair of a window at (y,x) */
short winpair_yx(WINDOW *win, int y, int x)
{
        cchar_t cch;
        wchar_t wch;
        short color;
        attr_t attr;

        mvwin_wch(win, y, x, &cch); /* Get the cch at (y,x) */
        getcchar(&cch, &wch, &attr, &color, NULL);

        return (color);
}

/**
 * bgcolor_yx -- return the background color of a window at (y,x) */
short bgcolor_yx(WINDOW *win, int y, int x)
{
        return (bgcolor(winpair_yx(win, y, x)));
}

/**
 * fgcolor_yx -- return the foreground color of a window at (y,x) */
short fgcolor_yx(WINDOW *win, int y, int x)
{
        return (fgcolor(winpair_yx(win, y, x)));
}


/**
 * take_bkgrnd -- modify a color pair so that it has the same bg as src 
 * @dst: the window which will be rendered with the new color pair
 * @src: the window whose background is to be taken
 * @pair: the color pair to be re-initialized with the new combination
 */
void take_bkgrnd(WINDOW *dst, WINDOW *src, short pair)
{
        short src_bg;
        short dst_fg;
        int y, x;

        getbegyx(dst, y, x);

        dst_fg = fgcolor(pair); 
        src_bg = bgcolor_yx(src, y, x);

        /* Re-init pair with src background */
        init_pair(pair, dst_fg, src_bg);
}






/**************************** CRAP THAT I HATE ********************************/
void geojug_start(void)
{
        /*GLOBE = malloc(sizeof(struct map_t)); // initialize the global map*/
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
