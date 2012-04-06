// vim:fdm=marker
/*******************************************************************************
 FILENAME: menus.c
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#include "../lib/llist/list.h"
#include "../lib/hash/hash.h"
#include "../gfx/gfx.h"
#include "../txt/psh.h"
#include "../stoc/stoc.h"
#include "../gfx/term.h"
#include "test.h"
#include "dialog.h"

#define _O_BOLD(win) wattron(win, A_BOLD)
#define _X_BOLD(win) wattroff(win, A_BOLD)
/******************************************************************************/
WINDOW *TALKW;
PANEL  *TALKP;

/* The psh command hash table */
HASHTABLE *CMD;
/******************************************************************************/
/******************************************************************************
 * Print a 30x30 test of the Perlin simplex noise generator, with the argument
 * specifying the boundary value at which a '#' character is exchanged for a
 * '~' character.
 ******************************************************************************/
/*void perlin_cb(void *word)*/
/*{*/
        /*const char *w = (const char *)word;*/

        /*test_simplex_noise((strtod(w, NULL)));*/
/*}*/
/******************************************************************************
 * Print the results of a fair roll of an n-sided die, where n is the argument
 * supplied by the word which immediately follows the pattern "roll". There
 * are some little confections as well, such as a for loop that returns the
 * first digit of a base-10 integer, using that information to test whether to
 * print the article "an" or "a", depending on the integer value of the roll.
 ******************************************************************************/
void diceroll_cb(void *word)
{
        const char *w = (const char *)word;

        int d = atoi(w);
        int r = roll_fair(d);

        /* Get first digit */
        int digit1 = r;
        for (;digit1>10; digit1/=10);

        char *an = "an";
        char *a = "a";
        char *article = ((r==11)||(digit1==8)) ? an : a;

        /* Print the roll, all fancy-like */
        wprintw(DIAGNOSTIC_WIN, "Rolling 1d%d...you rolled %s ", d, article);
        _O_BOLD(DIAGNOSTIC_WIN);
        wprintw(DIAGNOSTIC_WIN, "%d", r);
        _X_BOLD(DIAGNOSTIC_WIN);
        wprintw(DIAGNOSTIC_WIN, "!\n");
}
/******************************************************************************
 * Print a 18h x 54w map with the designated test data
 ******************************************************************************/
void mapgen_cb(void *test)
{
        const char *w = (const char *)test;
        int arg2;
        if      ((strcmp(w, "latitude")==0)||(strcmp(w, "lat")==0))     arg2 = 0;
        else if ((strcmp(w, "temperature")==0)||(strcmp(w, "temp")==0)) arg2 = 1;
        else if ((strcmp(w, "wind")==0))                                arg2 = 2;
        else if ((strcmp(w, "cell")==0))                                arg2 = 3;
        else if ((strcmp(w, "gradient")==0)||(strcmp(w, "grad")==0))    arg2 = 4;
        else return;
        testworld(3, arg2);
}
/******************************************************************************
 * Print the terminal's long name, short name, and associated attributes.
 ******************************************************************************/
void terminfo_cb(void *win)
{
        WINDOW *w = (WINDOW *)win;
        char *name = termname();
        char *info = longname();
        wprintw(w, "%s\n\n%s\n", name, info);
}

/******************************************************************************
 * Erase the window specified.
 ******************************************************************************/
void clear_cb(void *win)
{
        WINDOW *w = (WINDOW *)win;
        werase(w);
}

/* Initialize */
void init_menus(void)
{
        TALKW = newwin(3, COLS, LINES-3, 0);
        TALKP = new_panel(TALKW);
        hide_panel(TALKP);
        wcolor_set(TALKW, 0, NULL);
        wbkgd(TALKW, ' ');

        CMD = (HASHTABLE *)malloc(sizeof(HASHTABLE));
        HASHTABLE_INIT(CMD);
        ADD_TO_HASHTABLE(CMD, "roll", NULL, &diceroll_cb);
        /*ADD_TO_HASHTABLE(CMD, "perlin", NULL, &perlin_cb );*/
        ADD_TO_HASHTABLE(CMD, "mapgen", NULL, &mapgen_cb );
        ADD_TO_HASHTABLE(CMD, "clear", DIAGNOSTIC_WIN, &clear_cb);
        ADD_TO_HASHTABLE(CMD, "term", NULL, &checkterm);
        ADD_TO_HASHTABLE(CMD, "syspan", NULL, &toggle_syspan);
        ADD_TO_HASHTABLE(CMD, "workbox", NULL, &toggle_dpanel);
}
/******************************************************************************
 * This function parses each phrase in the context of the basic psh "shell",
 * that is, the actions of this function represent control or command bindings
 * of psh, not any child context, such as a program or applet. The individual 
 * actions are documented below in the course of the function.
 ******************************************************************************/
void psh_context(void *phrase)
{
        PHRASE *ph = (PHRASE *)phrase;

        HASHNODE *h;

        /* Set the active word to the last one in the list. */
        ph->prevword(ph);

        h = MATCH_HASH(CMD, ph->w->word);
        if ((h != NULL)) {
                if ((h->data != NULL))
                        h->cb(h->data);
                else {
                        ph->prevword(ph);
                        h->cb(ph->w->word);
                }
        }
        else speak_error("YOU GOT NULLS ON THE SKULL!");
}
/* Show/hide the main menu prompt */
void toggle_mm(void)
{
        TOGPAN(TALKP);
        wlisten(TALKW, &psh_context);
        TOGPAN(TALKP);
}
void toggle_workbox(void)
{
        TOGPAN(DIAGNOSTIC_PAN);
}
