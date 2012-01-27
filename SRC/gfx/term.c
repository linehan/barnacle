#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <term.h>
#include <panel.h>
#include <string.h>

#include "../pan/test.h"

#define PASS wprintw(SYSWIN, "[PASS]\n")
#define FAIL wprintw(SYSWIN, "[FAIL]\n")

static char * const XTERMS[] = {"xterm-256color","xterm-new","xterm-utf8","xtermc"};

void terminal_init(void)
{
        signed int *status = malloc(sizeof(signed int));
        int i = 0;
        char *tname = termname();
        if ((strcmp(tname, "xterm") == 0)) {
                do { setupterm(XTERMS[i++], 1, status);
                } while ((status==0) || (i>=4));
        }
        else initscr();
}

void checkterm(void)
{
        char *tname = termname();
        char *lname = longname();
        int baud = baudrate();

        wprintw(SYSWIN, "%s\n", tname);
        wprintw(SYSWIN, "%d baud\n\n", baud);
        wprintw(SYSWIN, "%s\n\n", lname);

        wprintw(SYSWIN, "Can INS and DEL chars?\t");
        if (has_ic()) PASS;
        else          FAIL;

        wprintw(SYSWIN, "Can INS and DEL lines?\t");
        if (has_il()) PASS;
        else          FAIL;

        wprintw(SYSWIN, "Can output in color?\t");
        if (has_colors()) PASS;
        else              FAIL;

        wprintw(SYSWIN, "Can change colors?\t");
        if (can_change_color()) PASS;
        else                     FAIL;
}
