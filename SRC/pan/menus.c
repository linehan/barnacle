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
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../gen/perlin.h"
#include "test.h"
/******************************************************************************/
#define SENTENCE_MAX 10 
#define SENTENCE_NPUNCT 3
#define PHRASE_NPUNCT 2
#define PHRASE_MAX 20
#define WORD_MAX 50

/* Print the prompt character on the desired window */
#define PROMPT(win) mvwprintw(win, 1, 0, " > ")
/* Erase WINDOW win and re-print the prompt string */
#define REPROMPT(win) { werase(win); \
                        PROMPT(win); \
                      }
/* Prepare the ncurses window for text entry */
#define PROMPT_ON(win) { PROMPT(win);    \
                         keypad(win, 0); \
                         echo();         \
                       }
/* Return the ncurses window to its previous state. */
#define PROMPT_OFF(win) { werase(win);    \
                          keypad(win, 1); \
                          noecho();       \
                        }
/* Return a pointer to a char array precisely as large as the argument. */
#define bigas(string) (char *)malloc(strlen(string) * sizeof(char))
/* Initialize an existing pointer to an array of char to the size of the
 * argument string. */
#define fit(dest, source) dest = bigas(source);
/* Allocate memory for and copy string 'source' to the (uninitialized)
 * char pointer 'dest' */
#define dup(dest, source) { fit(dest, source); strcpy(dest, source); }
/******************************************************************************/
struct catenae {
        char *dependency;
        struct list_head strings;
        struct list_node node;
};

struct strnode {
        char *word;
        struct list_node node;
};

WINDOW *TALKW, *TALKW;
PANEL *TALKP, *TALKP;
/******************************************************************************/
/* Initialize */
void menus_init(void)
{
        TALKW = newwin(3, COLS, LINES-3, 0);
        TALKP = new_panel(TALKW);
        hide_panel(TALKP);
        wcolor_set(TALKW, MENU, NULL);
        wbkgd(TALKW, ' ');
}
/* Process the \0-terminated input string into whitespace-separated tokens. 
 * A linked list is created to store the input, and strtok_r() is used to 
 * iterate over the input string, splitting it at each whitespace. The result 
 * (a word) is copied into a new node, and this node is appended to the linked 
 * list.
 *
 * Once any processing is completed, the linked list is looped over again,
 * and each node is first removed from the list, then free()'d, before the
 * function returns. */
void process_input(const char *input)
{
        char *BANG = "!";
        char *PERIOD = ".";
        char *QMARK = "?";
        char *COMMA = ",";

        char *word, *save;

        LIST_HEAD(mystring);
        struct strnode *tmp, *next; /* For iterating over the ll */

        /* Tokenize the input string */
        for (word = strtok_r(input, " ", &save);
             word;
             word = strtok_r(NULL, " ", &save))
        {
                struct strnode *new = malloc(sizeof(struct strnode));
                dup(new->word, word);
                list_add(&mystring, &new->node);
        }
        /* Test print */
        tmp = list_tail(&mystring, struct strnode, node);
                if ((strcmp(tmp->word, "clear") == 0))
                        werase(DIAGNOSTIC_WIN);
                else if ((strcmp(tmp->word, "perlin") == 0)) {
                      list_del_from(&mystring, &tmp->node);
                      list_add(&mystring, &tmp->node);
                      tmp = list_tail(&mystring, struct strnode, node);
                      test_simplex_noise(strtod(tmp->word, NULL));
                }
                else
                        tmp = NULL;
        
        /*list_for_each_rev(&mystring, tmp, node) {*/
                /*[>wprintw(DIAGNOSTIC_WIN, "%s\n", tmp->word);<]*/


        /*}*/
        /* Safely delete and free() */
        list_for_each_safe(&mystring, tmp, next, node) {
                list_del_from(&mystring, &tmp->node);
                free(tmp);
        }
}
/* Receive a line of text */
void receive_text(void)
{
        char buf[100];
        char *c;
        PROMPT_ON(TALKW);
        while (((c = getch())!= '`')) {
                ungetch(c);
                wgetstr(TALKW, buf);
                if ((strcmp(buf, "clear") == 0)) {
                        REPROMPT(TALKW);
                }
                process_input(buf);
                REPROMPT(TALKW);
        }
        hide_panel(TALKP);
        PROMPT_OFF(TALKW);
}
/* Show/hide the main menu prompt */
void toggle_mm(void)
{
        if ((panel_hidden(TALKP))) {
                show_panel(TALKP);
                receive_text();
        }
        else 
                hide_panel(TALKP);
}
