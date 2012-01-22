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
#include "test.h"
/******************************************************************************/
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

struct strings {
        char *string;
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
/* Process the input (line) buffer into finer and finer syntactic elements.
 * The "morpho-syntactic hierarchy goes like this:
 * 
 *   Text --> Sentence/Clause --> Phrase --> Word --> Morpheme
 *
 * The input received by the function corresponds to a single line input at
 * the prompt, which is the "text". Various stages of this function will sift
 * it into its various parts. */
void process_input(const char *input)
{
        /* Texts are divided into sentences and clauses by the application of
         * certain punctuation */
        char *match = "!,.?:;";
        char *hopper, *save;
        char *sentence, *phrase, *word;

        dup(sentence, input);
        fit(hopper, input);

        for (phrase = strtok_r(input, match, &save);
             phrase;
             phrase = strtok_r(NULL, match, &save))
        {
                wprintw(DIAGNOSTIC_WIN, "%s\n", phrase);
                hopper = strstr(phrase, "who");
                if ((hopper != NULL))
                        wprintw(DIAGNOSTIC_WIN, "%s\n", hopper);
        }
}
/* Receive a line of text */
void get_stuff(void)
{
        char buf[800];
        char *c;
        mvwprintw(TALKW, 1, 0, " > ");
        keypad(TALKW, 0);
        echo();
        while (((c = getch())!= '`')) {
                ungetch(c);
                wgetstr(TALKW, buf);
                if ((strcmp(buf, "clear") == 0)) {
                        werase(TALKW);
                        mvwprintw(TALKW, 1, 0, " > ");
                }
                process_input(buf);
                werase(TALKW);
                mvwprintw(TALKW, 1, 0, " > ");
        }
        hide_panel(TALKP);
        werase(TALKW);
        keypad(TALKW, 1);
        noecho();
}
/* Show/hide the main menu prompt */
void toggle_mm(void)
{
        if ((panel_hidden(TALKP))) {
                show_panel(TALKP);
                get_stuff();
        }
        else 
                hide_panel(TALKP);
}
