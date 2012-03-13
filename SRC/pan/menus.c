// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <wchar.h>
#include <string.h>
#include <stdbool.h>

#include "../gen/dice.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../guy/model/guy.h"
#include "../guy/model/vitals.h"
#include "../guy/model/jobs.h"
#include "../guy/model/attributes.h"
#include "../guy/model/names.h"
#include "test.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define KEY_ESC 27 /* char value of ESC key */

// MODES
//------------------------------------------------------------------------------

enum operator_modes { 
        EXITING,
        STARTING,
        ATTRIBUTES,
        PROFESSION,
        VITALS,
        PATTERN,
        ACTION,
};

#define NMODES 23
#define RESET -1 

int mode;
int mode_changed;

inline void setmode(int newmode)
{
        if (mode == newmode) return;
        if (newmode == RESET) mode_changed ^= 1;
        else {
                mode = newmode;
                mode_changed = 1;
        }
}


// DIMENSIONS
//------------------------------------------------------------------------------

#define MENU_W 20
#define MENU_H (LINES/3)
#define MENU_X 1
#define MENU_Y LINES-MENU_H

#define DOCK_W COLS
#define DOCK_H 1
#define DOCK_X 0
#define DOCK_Y LINES-DOCK_H


// HELPER FUNCS
//------------------------------------------------------------------------------

#define WPARTS 3
enum panel_parts { BORD, BODY, BUFF };

PANEL *purple_panel(WINDOW *win[WPARTS], int h, int w, int y, int x)
{
        #define COLOR PUR_PUR
        #define FILL &PURPLE[2]
        #define hshrink(h) (h-3)
        #define wshrink(w) (w-2)

        PANEL *new;
        int i;

        win[BORD] = newwin(h, w, y, x);
        new       = new_panel(win[BORD]); 
        hide_panel(new); // No peeking

        win[BODY] = derwin(win[BORD], hshrink(h), hshrink(w), 1, 1);
        win[BUFF] = derwin(win[BORD], 1, w, h-1, 0);
        wcolor_set(win[BODY], PUR_PUR, NULL);

        keypad(win[BODY], 1);
        wbkgrnd(win[BORD], FILL);
        wbkgrnd(win[BODY], FILL);
        wbkgrnd(win[BUFF], FILL);
        vrt_refresh();

        return (new);
}

// THE MENUS 
//------------------------------------------------------------------------------

// 00. THE DOCK BOX
//------------------------------------------------------------------------------
WINDOW *dockbox_win;
WINDOW *dockbox_nam;
WINDOW *dockbox_buf;
PANEL  *dockbox_pan;
PANEL  *crew_pan;

void post_dockbox(void)
{
        if (dockbox_pan != NULL) {
                top_panel(dockbox_pan);
                if (!panel_hidden(crew_pan))
                        top_panel(crew_pan);
        }
        else {
                dockbox_win = newwin(DOCK_H, DOCK_W, DOCK_Y, DOCK_X);
                dockbox_nam = derwin(dockbox_win, 1, 20, 0, 1);
                dockbox_buf = derwin(dockbox_win, 1, COLS-25, 0, 23);
                wbkgrnd(dockbox_win, &PURPLE[2]);
                wbkgrnd(dockbox_nam, &PURPLE[2]);
                wbkgrnd(dockbox_buf, &PURPLE[2]);
                dockbox_pan = new_panel(dockbox_win);
        }
}


// 00. THE CREW MENU
//------------------------------------------------------------------------------
#define CREW_W 20

WINDOW *crew_win[WPARTS];
MENU   *crew_menu;

void post_menu_crew(void)
{
        int i;
        int n = nguys();
        char **name;
        ITEM **item;

        if (crew_pan == NULL)
                crew_pan = purple_panel(crew_win, MENU_H, MENU_W+1, MENU_Y, MENU_X);

        /* Collect the menu items. */
        name = calloc(n, sizeof(char *));
        for (i=0; i<n; i++) {
                name[i] = flname(muster[i]);
        }

        /* Build the menu. */
        item = calloc(n+1, sizeof(ITEM *));
        for (i=0; i<n; i++) {
                item[i] = new_item(name[i], name[i]);
                set_item_userptr(item[i], &muster[i]);
        }
        item[n] = (ITEM *)NULL;

        crew_menu = new_menu((ITEM **)item);

        /* Configure and post the menu */
        set_menu_win(crew_menu, crew_win[BORD]);
        set_menu_sub(crew_menu, crew_win[BODY]);
        set_menu_fore(crew_menu, COLOR_PAIR(PUR_YEL));
        set_menu_back(crew_menu, COLOR_PAIR(PUR_PUR));
        menu_opts_off(crew_menu, O_SHOWDESC);

        set_menu_format(crew_menu, MENU_H-2, 1);
        set_menu_mark(crew_menu, "");

        post_menu(crew_menu);
}



//------------------------------------------------------------------------------


inline void put_attr(WINDOW *win, int ofs, uint32_t key)
{
        #define STRIDE 5
        static const short STANDOUT = PUR_GRE;
        static const short ORIGINAL = PUR_PUR;

        int val[8];
        int i;

        wcolor_set(win, STANDOUT, NULL);
        waddwstr(win, L"Σ    Φ    Δ    A    Ψ    W    Χ    Λ");
        wcolor_set(win, ORIGINAL, NULL);

        unpack_attributes(key, val);
        for (i=0; i<8; i++) {
                mvwprintw(win, 0, ofs+(i*STRIDE)+1, "%02u", val[i]);
        }
}


void operate_on(void *noun)
{
        #define OUT dockbox_buf
        const short STANDOUT = PUR_YEL;
        const short BRIGHTER = PUR_GRE;
        const short ORIGINAL = PUR_PUR;

        static int label_width;
        static char *label;
        static int a[8];

        uint32_t key = (noun!=NULL) ? *(uint32_t *)noun : 0;
       
        if (mode_changed) {
                if (mode != EXITING) {
                        werase(OUT);
                        win_refresh(OUT);
                }
                setmode(RESET);
        }

        static int not_again;
        int i;
        if (not_again != -1) {
                for (i=0; i<nguys(); i++) {
                        set_vital(muster[i], HP, roll_fair(8));
                        set_vital(muster[i], SP, roll_fair(8));
                        set_vital(muster[i], LP, roll_fair(8));
                        set_vital(muster[i], EP, roll_fair(8));
                        focus(muster[i]);
                        focused->positive = true;
                        focused->xpulse = vit_blocklen(muster[i]);
                }
                not_again = -1;
        }

        switch(mode) {
        case EXITING:
                werase(dockbox_nam);
                wcolor_set(dockbox_nam, BRIGHTER, NULL);
                wprintw(dockbox_nam, "%s", flname(key));
                win_refresh(dockbox_nam);
                return;
        case ATTRIBUTES:
                wcolor_set(OUT, ORIGINAL, NULL);
                put_attr(OUT, label_width, key);
                break;
        case ACTION:
                focus(key);
                focused->action = 1;
        case VITALS:
                werase(OUT);
                wcolor_set(OUT, ORIGINAL, NULL);
                vit_print_blocks(OUT, label_width, key);
                break;
        case PROFESSION:
                wprintw(OUT, "%s", get_job(key));
                break;
        }
        werase(dockbox_nam);
        wcolor_set(dockbox_nam, STANDOUT, NULL);
        wprintw(dockbox_nam, "%s", flname(key));
        win_refresh(dockbox_nam);
        win_refresh(OUT);
}


void choose_noun(void) 
{
        /* ncurses sets the ESC key delay at 100ms by default, and this
         * is way too slow. According to Wolfram Alpha, 
         * 60hz == 0.0167s == 16.7ms, and is about equivalent to the time 
         * it takes a nerve impulse to travel the length of an avg. human 
         * body. For comparison, a typical human blink duration is 100ms-
         * 400ms, and that's just not what we're looking for here. */
        if (getenv ("ESCDELAY") == NULL) ESCDELAY = 25;

        #define DEFAULT_MODE VITALS

        ITEM *item = NULL;
        int c;

        // First run
        setmode(DEFAULT_MODE);
        item=current_item(crew_menu);
        operate_on(item_userptr(item));

        while ((c = getch())) 
        {
                switch (c)
                {
                case '/': 
                        TOGPAN(crew_pan);
                        wbkgrnd(crew_win[BUFF], &PURPLE[1]);
                        wprintw(crew_win[BUFF], " /"); 
                        win_refresh(crew_win[BUFF]);
                        scr_refresh();
                        while ((c=getch()) != '\n') {

                                if (c==KEY_ESC) {
                                        if (*(menu_pattern(crew_menu)) == '\0')
                                                goto end_pattern;
                                        else
                                                menu_driver(crew_menu, REQ_CLEAR_PATTERN);
                                }

                                if (c==KEY_BACKSPACE)
                                        menu_driver(crew_menu, REQ_BACK_PATTERN);

                                menu_driver(crew_menu, c);

                                werase(crew_win[BUFF]);
                                wprintw(crew_win[BUFF], " /%s", menu_pattern(crew_menu));

                                item=current_item(crew_menu);
                                operate_on(item_userptr(item));

                                win_refresh(crew_win[BUFF]);
                                scr_refresh();
                        }
                        end_pattern:
                        menu_driver(crew_menu, REQ_CLEAR_PATTERN);
                        werase(crew_win[BUFF]);
                        wbkgrnd(crew_win[BUFF], &PURPLE[3]);
                        win_refresh(crew_win[BUFF]);
                        break;
                case '!':
                        setmode(ACTION);
                        break;
                case 'k':
                        menu_driver(crew_menu, REQ_PREV_ITEM);
                        break;
                case 'j':
                        menu_driver(crew_menu, REQ_NEXT_ITEM);
                        break;
                case 'n':
                        menu_driver(crew_menu, REQ_PREV_MATCH);
                        break;
                case 'p':
                        menu_driver(crew_menu, REQ_NEXT_MATCH);
                        break;
                case 'P':
                        setmode(PROFESSION);
                        break;
                case 's':
                        setmode(ATTRIBUTES);
                        break;
                case 'v':
                        setmode(VITALS);
                        break;
                case 'O':
                        TOGPAN(crew_pan);
                        vrt_refresh();
                        scr_refresh();
                        break;
                case KEY_ESC:
                case 'o':
                        setmode(EXITING);
                        hide_panel(crew_pan);
                        operate_on(item_userptr(item));
                        vrt_refresh();
                        scr_refresh();
                        return;
                }
                item=current_item(crew_menu);
                operate_on(item_userptr(item));

                scr_refresh();
        }
}


void toggle_menu_crew(void)
{
        scr_refresh();
        choose_noun();
}
