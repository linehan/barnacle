#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include "gloss.h"

////////////////////////////////////////////////////////////////////////////////
inline void revealc(WINDOW *win, char *str, short color, int L)
{
        int i;

        wcolor_set(win, color, NULL);

        for (i=0; i<L; i++) {
                wprintw(win, "%c", str[i]);
                wrefresh(win);
        }
}
inline void revealw(WINDOW *win, wchar_t *wcs, short color, int L)
{
        cchar_t cch;
        int i;

        for (i=0; i<L; i++) {
                setcchar(&cch, &wcs[i], 0, color, NULL);
                wadd_wch(win, &cch);
                wrefresh(win);
        }
}
////////////////////////////////////////////////////////////////////////////////
void reveal(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        if (msg->wcs!=NULL) revealw(msg->win, msg->wcs, msg->co, L);
        /*else                revealc(msg->win, msg->str, msg->co, L);*/

        doupdate();
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
inline void unrevealc(WINDOW *win, char *str, int len, short color, int L)
{
        int i;

        wcolor_set(win, color, NULL);

        wmove(win, 0, L);

        for (i=L; i<len; i++) {
                wprintw(win, "%c", str[i]);
                wrefresh(win);
        }
}
inline void unrevealw(WINDOW *win, wchar_t *wcs, int len, short color, int L)
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
////////////////////////////////////////////////////////////////////////////////
void unreveal(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        if (msg->wcs!=NULL) unrevealw(msg->win, msg->wcs, msg->len, cuco(msg->win), L);
        /*else                unrevealc(msg->win, msg->str, msg->len, cuco(msg->win), L);*/

        doupdate();
}
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
inline void shinec(WINDOW *win, char *str, int len, short color, int L)
{
        wcolor_set(win, color, NULL);

        mvwprintw(win, 0, L-1, "%c", str[L-1]);
        wrefresh(win);
}
inline void shinew(WINDOW *win, wchar_t *wcs, int len, short color, int L)
{
        cchar_t cch;

        setcchar(&cch, &wcs[L-1], 0, color, NULL);
        mvwadd_wch(win, 0, L-1, &cch);
        wrefresh(win);
}
////////////////////////////////////////////////////////////////////////////////
void shine(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        if (msg->wcs!=NULL) shinew(msg->win, msg->wcs, msg->len, msg->hi, L);
        /*else                shinec(msg->win, msg->str, msg->len, msg->hi, L);*/
}
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
inline void pushrc(WINDOW *win, char *str, int len, short color, int L)
{
        int i;

        wmove(win, 0, L);
        wcolor_set(win, color, NULL);

        for (i=0; i<(len-L); i++) {
                wprintw(win, "%c", str[i]);
                wrefresh(win);
        }
}
inline void pushrw(WINDOW *win, wchar_t *wcs, int len, short color, int L)
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
////////////////////////////////////////////////////////////////////////////////
void pushr(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        if (msg->wcs!=NULL) pushrw(msg->win, msg->wcs, msg->len, cuco(msg->win), L);
        /*else                pushrc(msg->win, msg->str, msg->len, cuco(msg->win), L);*/

        doupdate();
}
////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
inline void pushlc(WINDOW *win, char *str, int len, short color, int L)
{
        int i;

        wcolor_set(win, color, NULL);

        for (i=L; i<len; i++) {
                wprintw(win, "%c", str[i]);
                wrefresh(win);
        }
}
inline void pushlw(WINDOW *win, wchar_t *wcs, int len, short color, int L)
{
        cchar_t cch;
        int i;

        for (i=0; i<len; i++) {
                setcchar(&cch, &wcs[i], 0, color, NULL);
                wadd_wch(win, &cch);
                wrefresh(win);
        }
}
////////////////////////////////////////////////////////////////////////////////
void pushl(void *self, int L)
{
        struct gloss_t *msg = (struct gloss_t *)self;

        werase(msg->win);
        wrefresh(msg->win);

        if (msg->wcs!=NULL) pushlw(msg->win, msg->wcs, msg->len, cuco(msg->win), L);
        /*else                pushlc(msg->win, msg->str, msg->len, cuco(msg->win), L);*/

        doupdate();
}
////////////////////////////////////////////////////////////////////////////////







inline bool fadeout(WINDOW *win, const char *str)
{
        #define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
        static uint32_t mask = 0x00000001;
        static uint32_t card;
        static int len;
        static bool ready;
        int i, tmp;
        char c;

        if (ready == false) {
                len = strlen(str);
                card = (~0<<len);
                ready = true;
        }

        if (card == ~(0)) ready = false;

        tmp = ones(card);

        while (tmp == ones(card) && ready == true)
                card |= (mask<<(roll_fair(len)));

        werase(win);
        wrefresh(win);
        for (i=0; i<10; i++) {
                c = (CHECK_BIT(card, i)!=0) ? ' ' : str[i];
                wprintw(win, "%c", c);
        }
        wrefresh(win);

        return (ready);
}


