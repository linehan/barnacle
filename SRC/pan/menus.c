// vim:fdm=marker
/*******************************************************************************
 FILENAME: menus.c
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

#include "../lib/llist/list.h"
#include "../lib/hash/hash.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../gen/perlin.h"
#include "../txt/psh.h"
#include "../gen/dice.h"
#include "test.h"
#include "menus.h"

#define _O_BOLD(win) wattron(win, A_BOLD)
#define _X_BOLD(win) wattroff(win, A_BOLD)
/******************************************************************************/
WINDOW *TALKW, *TALKW;
PANEL *TALKP, *TALKP;

const char BANG = '!';
const char PERIOD = '.';
const char QMARK = '?';
const char COMMA = ',';

/* The psh command hash table */
HASHTABLE *CMD;
/******************************************************************************/
void perlin_cb(void *word)
{
        const char *w = (const char *)word;

        test_simplex_noise((strtod(w, NULL)));
}

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

void clear_cb(void *win)
{
        WINDOW *w = (WINDOW *)win;
        werase(w);
}

/* Initialize */
void menus_init(void)
{
        TALKW = newwin(3, COLS, LINES-3, 0);
        TALKP = new_panel(TALKW);
        hide_panel(TALKP);
        wcolor_set(TALKW, MENU, NULL);
        wbkgd(TALKW, ' ');

        CMD = (HASHTABLE *)malloc(sizeof(HASHTABLE));
        HASHTABLE_INIT(CMD);
        ADD_TO_HASHTABLE(CMD, "roll", &diceroll_cb);
        ADD_TO_HASHTABLE(CMD, "perlin", &perlin_cb);
        ADD_TO_HASHTABLE(CMD, "clear", &clear_cb);
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
                ph->prevword(ph);
                h->cb(ph->w->word);
        }
        else speak_error("YOU GOT NULLS ON THE SKULL!");
}

/* Show/hide the main menu prompt */
void toggle_mm(void)
{
        if ((panel_hidden(TALKP))) {
                show_panel(TALKP);
                doupdate();
                wlisten(TALKW, &psh_context);
                hide_panel(TALKP);
                doupdate();
        }
        else 
                hide_panel(TALKP);
                doupdate();
}
