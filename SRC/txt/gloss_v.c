#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include "gloss.h"



////////////////////////////////////////////////////////////////////////////////
inline void wcs_reveal(WINDOW *win, wchar_t *wcs, short color, int L)
{
        cchar_t cch;
        int i;

        wcolor_set(win, color, NULL);

        for (i=0; i<L; i++) {
                setcchar(&cch, &wcs[i], 0, color, NULL);
                wadd_wch(win, &cch);
                wrefresh(win);
        }
}
/*
 *
 *
 */
void reveal(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        wcs_reveal(msg->win, msg->wcs, msg->co, L);

        doupdate();
}





////////////////////////////////////////////////////////////////////////////////
inline void wcs_unreveal(WINDOW *win, wchar_t *wcs, int len, short color, int L)
{
        cchar_t cch;
        int i;

        wmove(win, 0, L);

        for (i=L; i<len; i++) {
                setcchar(&cch, &wcs[i], 0, color, NULL);
                wadd_wch(win, &cch);
                wrefresh(win);
        }
}
/*
 *
 *
 */
void unreveal(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        wcs_unreveal(msg->win, msg->wcs, msg->len, cuco(msg->win), L);

        doupdate();
}



////////////////////////////////////////////////////////////////////////////////
inline void wcs_shine(WINDOW *win, wchar_t *wcs, int len, short color, int L)
{
        cchar_t cch;

        wcolor_set(win, color, NULL);

        setcchar(&cch, &wcs[L-1], 0, color, NULL);
        mvwadd_wch(win, 0, L-1, &cch);
        wrefresh(win);
}
/*
 *
 *
 */
void shine(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        wcs_shine(msg->win, msg->wcs, msg->len, msg->hi, L);
}




////////////////////////////////////////////////////////////////////////////////
inline void wcs_pushr(WINDOW *win, wchar_t *wcs, int len, short color, int L)
{
        cchar_t cch;
        int i;

        wmove(win, 0, L);

        for (i=0; i<(len-L); i++) {
                setcchar(&cch, &wcs[i], 0, color, NULL);
                wadd_wch(win, &cch);
                wrefresh(win);
        }
}
/*
 *
 *
 */
void pushr(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        wcs_pushr(msg->win, msg->wcs, msg->len, cuco(msg->win), L);

        doupdate();
}






////////////////////////////////////////////////////////////////////////////////
inline void wcs_pushl(WINDOW *win, wchar_t *wcs, int len, short color, int L)
{
        cchar_t cch;
        int i;

        for (i=0; i<len; i++) {
                setcchar(&cch, &wcs[i], 0, color, NULL);
                wadd_wch(win, &cch);
                wrefresh(win);
        }
}
/*
 *
 *
 */
void pushl(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        wcs_pushl(msg->win, msg->wcs, msg->len, cuco(msg->win), L);

        doupdate();
}



////////////////////////////////////////////////////////////////////////////////
inline void wcs_dropout(WINDOW *win, wchar_t *wcs, int len, short color, int L)
{
        #define BUFMAX 100
        static wchar_t buf[BUFMAX];
        static int count;
        cchar_t cch;
        int steps;
        int i;

        /* Copy a new string to the buffer on the first loop */
        if (L==0) {
                wmemcpy(buf, wcs, len * sizeof(wchar_t));
                for (i=0, count=len; i<len; i++) {
                        if (iswspace(buf[i])) count--;
                }
        }

        /* Roll how many chars to skip before removing one */
        steps = roll_fair(--count);
 
        for (i=0;;) {                           ///
                if (!iswspace(buf[i])) steps--;  //
                if (steps>=0) i++;               // 
                else {                           // forgive me
                        buf[i]=L' ';             //
                        break;                   // 
                }                               ///
        }

        /* Print the perforated buffer */
        for (i=0; i<len; i++) {
                setcchar(&cch, &buf[i], 0, color, NULL);
                wadd_wch(win, &cch);
                wrefresh(win);
        }
}
/*
 *
 * 
 */
void dropout(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        wcs_dropout(msg->win, msg->wcs, msg->len, cuco(msg->win), L);

        doupdate();
}




