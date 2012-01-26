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
#include "../txt/psh.h"
#include "../gen/dice.h"
#include "test.h"
#include "menus.h"
/******************************************************************************/
WINDOW *TALKW, *TALKW;
PANEL *TALKP, *TALKP;

const char BANG = '!';
const char PERIOD = '.';
const char QMARK = '?';
const char COMMA = ',';
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

int get_first_digit(int number)
{
        while (number > 10) {
                number /= 10;
        }
        return number;
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
void process_input(PHRASE *ph)
{
        ph->prevword(ph);

        IFMATCH(ph->w->word, "clear") 
                werase(DIAGNOSTIC_WIN);
        ELMATCH(ph->w->word, "workbox")
                toggle_dpanel();
        ELMATCH(ph->w->word, "perlin") {
                ph->prevword(ph);
                test_simplex_noise(strtod(ph->w->word, NULL));
        }
        ELMATCH(ph->w->word, "roll") {
                ph->prevword(ph);
                int d = atoi(ph->w->word);
                int r = roll_fair(d);
                char article[3];

                if (((r == 11)||(get_first_digit(r)) == 8))
                        strcpy(article, "an");
                else
                        strcpy(article, "a");

                wprintw(DIAGNOSTIC_WIN, "Rolling ");
                wattron(DIAGNOSTIC_WIN, A_BOLD);
                wprintw(DIAGNOSTIC_WIN, "1d%d", d);
                wattroff(DIAGNOSTIC_WIN, A_BOLD);
                wprintw(DIAGNOSTIC_WIN, "...you rolled %s ", article);
                wattron(DIAGNOSTIC_WIN, A_BOLD);
                wprintw(DIAGNOSTIC_WIN, "%d", r);
                wattroff(DIAGNOSTIC_WIN, A_BOLD);
                wprintw(DIAGNOSTIC_WIN, "!\n");
        }
}
void receive_text(void)
{
        char buffer[MAX_PHRASE_LEN];
        short peek;
        STRING *str = new_string();

        PROMPT_ON(TALKW);
        while ((peek = wgetch(TALKW)) != '`') {
                if ((peek == KEY_UP)) {
                       /* As long as the user keeps pressing KEY_UP,
                        * don't call ungetch(), but cycle backwards
                        * through the phraselist and print each phrase
                        * on the prompt. */
                        do {    REPROMPT(TALKW);
                                if (str->nextphrase(str))
                                        wprintw(TALKW, "%s ", str->p->phrase);
                        } while ((peek = wgetch(TALKW)) == KEY_UP);
                       /* Since the last peek was the one which broke the
                        * do loop, it follows that it's probably actual
                        * input that belongs in the input stream, so that's
                        * where we send it. */
                        ungetch(peek);
                       /* Next, we feed the phrase we've selected back into
                        * the input stream as well. This is an important
                        * point -- although we printed this phrase in the
                        * do loop above, the line buffer cannot see it. The
                        * line buffer consists entirely of characters from
                        * an input stream opened during a call to wgetch()
                        * or wgetstr(), et al.
                        *
                        * The function ungetch() places its argument in the 
                        * input stream, and there it sits until another 
                        * function like wgetch() opens the input stream and 
                        * captures it, along with any other chars that have 
                        * accumulated, as though the user had typed them in 
                        * almost instantaneously. */
                        if (str->p != NULL) {
                                int i = (str->p->l);
                                while (i--) ungetch(str->p->phrase[i]);
                        }
                       /* Now we can proceed normally. */
                }
                else ungetch(peek);
                
                REPROMPT(TALKW);
               /* Open the input string and receive a string of length
                * MAX_PHRASE_LEN in the buffer. Because the function
                * wants to get a *string*, it will loiter at the input
                * stream until it receives a newline. */
                wgetnstr(TALKW, buffer, MAX_PHRASE_LEN);
               /* As long as the buffer (the line) wasn't empty, create
                * a new PHRASE node for it and send the node to the
                * processing function. Once the processing function returns
                * successfully, go ahead and add the node to the phraselist. */
                if (buffer[0] != '\0') {
                        PHRASE *ph = new_phrase(buffer);
                        process_input(ph);
                        ADD_PHRASE(ph, str);
                }
               /* Erase the prompt and loop. */
                REPROMPT(TALKW);
        }
        hide_panel(TALKP);
        PROMPT_OFF(TALKW);
       /* Unspool the STRING we've been working with. */
        str->unspool(str);
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
