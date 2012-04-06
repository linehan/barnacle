// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>

#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../gen/name/name.h"
#include "ctrlpanels.h"

#define OUTPUTW INFOW[BODY]
#define HEADERW INFOW[HEAD]
#define PRINT_KEY '\n'
#define BREAK_KEY 'c'

void crewgen(int n)
{
        const char *crew[n];
        char c;
        int i;

        werase(HEADERW);
        wprintw(HEADERW, "Press ENTER to generate a crew of %u.", n);

        TOGGLE_INFO;
        scr_refresh();

        while ((c = getch())) {

                if (c == BREAK_KEY) break;
                if (c != PRINT_KEY) continue;

                werase(OUTPUTW);

                for (i=0; i<n; i++) {
                        crew[i] = pick_surname();
                        wprintw(OUTPUTW, "%s\n", crew[i]);
                }
        }
        TOGGLE_INFO;
}


