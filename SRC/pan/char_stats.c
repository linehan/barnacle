#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <form.h>
#include <wchar.h>
#include <locale.h>

#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../gfx/wselect.h"
#include "../gen/name/name.h"
#include "ctrlpanels.h"
#include "../kin/k_character.h"
#include "../mob/pc.h"
#include "../gfx/fancy_menus.h"
#include "button.h"
#include "test.h"

#define KEY_ESC 27 // Char value of ESC

PANEL  *cstat_pan;
WINDOW *cstat_win[4];

void init_cstat(void)
{
        cstat_pan = make_purple_panel(cstat_win, 25, COLS/4, 0, COLS-(COLS/4));
}

void post_cstat(void) 
{
        struct fancy_menu *persons;

        int i;
        int n = npersons();
        if (n == 0) return;

        char **names = calloc(n, sizeof(char *));

        for (i=0; i<n; i++) {
                if (person_list[i] == NULL) continue;
                names[i] = fullname(person_list[i]);
                wprintw(DIAGNOSTIC_WIN, "%s\n", names[i]);
        }

        persons = new_fancy_menu(names, names, PUR_GRE, PUR_PUR);
        put_fancy_menu(persons, cstat_win[BODY], 1, 45, 10, 15);
        set_fancy_select(persons, PUR_YEL);
        post_fancy_menu(persons);
}

