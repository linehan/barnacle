#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <term.h>
#include <panel.h>
#include <string.h>

#include "../test/test.h"

#define PASS wprintw(SYSWIN, "[PASS]\n")
#define FAIL wprintw(SYSWIN, "[FAIL]\n")

static char const *XTERM_MODE = "xterm-256color";
static char const *RXVT_MODE = "rxvt-unicode-256color";

int terminal_check(void)
{
        signed int *status = malloc(sizeof(signed int));
        int i = 0;
        char *tname = termname();
        if ((strcmp(tname, "xterm") == 0)) {
                speak_error_mono("\n\tHey! We don't serve their kind here!\n\n"
                                 "\tWhat?\n\n"
                                 "\tYour pathetic terminal modes! They'll have"
                                 " to wait until the devs catch up to"
                                 " rxvt-unicode!");
                return 0;
                /*do { setupterm(XTERMS[i++], 1, status);*/
                /*} while ((status==0) || (i>=4));*/
        }
        return 1;
}

void checkterm(void)
{
        char *tname = termname();
        char *lname = longname();
        int baud = baudrate();

        wprintw(SYSWIN, "%s\n\n", lname);
        wprintw(SYSWIN, "%s\n", tname);
        wprintw(SYSWIN, "%d baud\n\n", baud);


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
