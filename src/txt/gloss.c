#include <stdlib.h>
#include "../com/arawak.h"
#include "gloss.h"
#include "../lib/textutils.h"


/* DESTRUCTOR
 * del_gloss -- free the gloss object and its members
 */
void del_gloss(struct gloss_t *g)
{
       free(g->wcs);
}


/* CONSTRUCTOR
 * new_gloss -- create a new gloss object
 * @win: window to print the to 
 * @wcs: string to print
 * @co:  normal color pair
 * @hi:  highlight color pair
 */
struct gloss_t *new_gloss(WINDOW *win, wchar_t *wcs, short co, short hi)
{
        struct gloss_t *new = malloc(sizeof(struct gloss_t));

        new->win = win;
        new->wcs = wcsdup(wcs);
        new->len = wcslen(wcs);

        new->co = co;
        new->hi = hi;

        new->put[REVEAL]   = &method_reveal;
        new->put[UNREVEAL] = &method_unreveal;
        new->put[SHINE]    = &method_shine;
        new->put[PUSH_R]   = &method_pushr;
        new->put[PUSH_L]   = &method_pushl;
        new->put[DROPOUT]  = &method_dropout;

        return (new);
}


/* CONSTRUCTOR 
 * If you have a 7-bit ASCII char * string, this function will properly
 * convert it into a wide-character sequence suitable for storage in
 * the struct gloss_t.
 */
struct gloss_t *str_gloss(WINDOW *win, char *str, short co, short hi)
{
        wchar_t *wcs = mbdup(str);

        return wcs ? new_gloss(win, wcs, co, hi) : NULL;
}




/*
 * TEXT EFFECTS SHOULD HAVE 4 STAGES
 *      0. Appear 
 *      1. Stabilize
 *      2. Decay
 *      3. Disappear
 *
 *      Each of these stages has a number of loops to id its duration,
 *      which may also receive their input from a randomizer.
 *
 *      You pass say() an option which is switched to set a mode. Once
 *      the mode is set, it must go to completion unless it is interrupted.
 *      The mode will proceed from 0...3, and release the mode lock after
 *      it has completed.
 */
bool gloss(struct gloss_t *test)
{
        /*#define SKIP_LOOP(x) if ((SKIP++ % (x))!=0) return false*/
        #define ON_LOOP(x)      if ((LOOP++) == (x))
        #define  REPORT(x)      STATUS=(x); break
        #define   YIELD(x)      LOOP=0, STAGE=(x)
        #define     UNLOOP      STAGE=LOOP=0; 

        #define LOOPVAR(v) v = (LOOP)

        static unsigned int LOOP;
        static unsigned int STAGE;
        static unsigned int SKIP;
        static bool STATUS;
        int L;

        LOOPVAR(L);

        switch (STAGE) {

        case 0: ON_LOOP(test->len) YIELD(1);
                test->put[REVEAL](test, L);
                REPORT(true);

        case 1: ON_LOOP(test->len) YIELD(2);
                test->put[SHINE](test, L);
                REPORT(true);

        case 2: ON_LOOP(30) YIELD(3);
                REPORT(true);

        case 3: ON_LOOP(test->len) YIELD(4);
                test->put[PUSH_R](test, L);
                REPORT(true);

        case 4: UNLOOP;
                REPORT(false);

        }

        return (STATUS);
} 

