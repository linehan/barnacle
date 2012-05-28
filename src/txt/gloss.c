/* 
 * gloss.c -- Objects for pretty-printing text. 
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdlib.h>
#include "../com/barnacle.h"
#include "gloss.h"
#include "../lib/textutils.h"
#include "effects.h"

/* DATA STRUCTURE 
``````````````````````````````````````````````````````````````````````````````*/
struct gloss_t {
        WINDOW *win;
        const wchar_t *wcs;
        size_t len;
        short co;
        short hi;
};



/* ACCESSORS 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * gloss_co -- return the color pair number of a gloss object
 * @gloss: pointer to a gloss object
 */
short gloss_co(struct gloss_t *gloss)
{
        return (gloss->co);
}
/**
 * gloss_hi -- return the highlight color pair number of a gloss object
 * @gloss: pointer to a gloss object
 */
short gloss_hi(struct gloss_t *gloss)
{
        return (gloss->hi);
}





/* CORE FUNCTIONS 
``````````````````````````````````````````````````````````````````````````````*/
/* CONSTRUCTOR
 * new_gloss -- create a new gloss object
 * @win: window to print the to 
 * @wcs: string to print
 * @co:  normal color pair
 * @hi:  highlight color pair
 */
struct gloss_t *new_gloss(WINDOW *win, const wchar_t *wcs, short co, short hi)
{
        struct gloss_t *new = malloc(sizeof(struct gloss_t));

        new->win = win;
        new->wcs = wdup(wcs);
        new->len = wcslen(wcs);
        new->co  = co;
        new->hi  = hi;

        return (new);
}


/* DESTRUCTOR
 * del_gloss -- free the gloss object and its members
 */
void del_gloss(struct gloss_t *g)
{
        if (g->wcs)
                free(g->wcs);
                g->wcs = NULL;
                /*release((void **)&g->wcs);*/
}



/* GLOSS METHODS (see below) 
``````````````````````````````````````````````````````````````````````````````*/
void method_reveal(struct gloss_t *gloss, int L);
void method_unreveal(struct gloss_t *gloss, int L);
void method_shine(struct gloss_t *gloss, int L);
void method_pushr(struct gloss_t *gloss, int L);
void method_pushl(struct gloss_t *gloss, int L);
void method_dropout(struct gloss_t *gloss, int L);


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

        static int LOOP;
        static int STAGE;
        static int SKIP;
        static bool STATUS;
        int L;

        if (test == NULL) {
                UNLOOP;
                STATUS = false;
                return false;
        }

        LOOPVAR(L);

        switch (STAGE) {

        case 0: ON_LOOP(test->len) YIELD(1);
                method_reveal(test, L);
                REPORT(true);

        case 1: ON_LOOP(test->len) YIELD(2);
                method_shine(test, L);
                REPORT(true);

        case 2: ON_LOOP(30) YIELD(3);
                REPORT(true);

        case 3: ON_LOOP(test->len) YIELD(4);
                method_pushr(test, L);
                REPORT(true);

        case 4: UNLOOP;
                REPORT(false);

        }

        return (STATUS);
} 



/* GLOSS METHODS THAT CALL TEXT EFFECTS 
``````````````````````````````````````````````````````````````````````````````*/
/** 
 * METHOD REVEAL
 * method_reveal -- method for wcs_reveal
 */
void method_reveal(struct gloss_t *gloss, int L)
{
        werase(gloss->win);
        wrefresh(gloss->win);
        wcolor_set(gloss->win, gloss->co, NULL);
        wcs_reveal(gloss->win, gloss->wcs, gloss->len, L);
        doupdate();
}

/** 
 * METHOD UNREVEAL
 * method_unreveal -- method for wcs_unreveal
 */
void method_unreveal(struct gloss_t *gloss, int L)
{
        werase(gloss->win);
        wrefresh(gloss->win);
        wcs_unreveal(gloss->win, gloss->wcs, gloss->len, L);
        doupdate();
}

/**
 * METHOD SHINE
 * method_shine -- call wcs_shine on the gloss object
 */
void method_shine(struct gloss_t *gloss, int L)
{
        wcolor_set(gloss->win, gloss->hi, NULL);
        wcs_shine(gloss->win, gloss->wcs, gloss->len, L);
}

/**
 * METHOD PUSH RIGHT
 * method_pushr -- call the wcs_pushr method on the gloss object
 */
void method_pushr(struct gloss_t *gloss, int L)
{
        werase(gloss->win);
        wcs_pushr(gloss->win, gloss->wcs, gloss->len, L);
        doupdate();
}

/**
 * METHOD PUSH LEFT 
 * method_pushl -- call the wcs_pushl method on the gloss object
 */
void method_pushl(struct gloss_t *gloss, int L)
{
        werase(gloss->win);
        wrefresh(gloss->win);
        wcs_pushl(gloss->win, gloss->wcs, gloss->len, L);
        doupdate();
}

/**
 * METHOD DROPOUT
 * method_dropout -- call wcs_dropout on the gloss object
 */
void method_dropout(struct gloss_t *gloss, int L)
{
        werase(gloss->win);
        wrefresh(gloss->win);
        wcs_dropout(gloss->win, gloss->wcs, gloss->len, L);
        doupdate();
}

