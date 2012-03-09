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

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../mob/pc.h"
#include "test.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define KEY_ESC 27 /* char value of ESC key */
#define newpan new_panel



// Helper Functions
//##############################################################################

#define quW (COLS/4)        // Quarter width
#define thH (LINES/3)       /* 1/3 height of stdscr */
#define thW (COLS/3)        /* 1/3 width of stdscr */        
#define thL 0               /* Offset for leftmost third */
#define thM thW             /* Offset for middle third */
#define thR ((COLS)-(thW))  /* Offset for rightmost third */
#define thB ((LINES)-(thH)) /* Vertical offset to the bottom third */

#define WPARTS 2

/*
  Draw a box around a window using wide-character strings. 
 */
inline void wbox(WINDOW *win, 
                const wchar_t *ls,  // Left side
                const wchar_t *rs,  // Right side
                const wchar_t *ts,  // Top side
                const wchar_t *bs,  // Bottom side
                const wchar_t *ul,  // Upper-left corner
                const wchar_t *ur,  // Upper-right corner
                const wchar_t *bl,  // Bottom-left corner
                const wchar_t *br)  // Bottom-right corner
{
        int ymax;
        int xmax;
        int h;
        int w;
        int i;

        getmaxyx(win, h, w);
        ymax = h-1;
        xmax = w-1;

        /* Top */
        mvwaddwstr(win, 0, 0, ul);
        for (i=1; i<w; i++)        
                mvwaddwstr(win, 0, i, ts);
        mvwaddwstr(win, 0, xmax, ur);            

        /* Sides */
        for (i=1; i<h; i++) {
                mvwaddwstr(win, i, 0, ls);
                mvwaddwstr(win, i, xmax, rs);
        }

        /* Bottom */
        mvwaddwstr(win, ymax, 0, bl);            
        for (i=1; i<w; i++)        
                mvwaddwstr(win, ymax, i, bs);
        mvwaddwstr(win, ymax, xmax, br);            
}


enum panel_parts { BORD, BODY };

/*
  Paint a pretty purple panel to perfection.
*/
PANEL *purple_panel(WINDOW *win[WPARTS], int h, int w, int y, int x)
{
        #define COLOR PUR_PUR
        #define FILL &PURPLE[2]
        #define hshrink(h) (h-3)
        #define wshrink(w) (w-2)

        PANEL *new;
        int i;

        win[BORD] = newwin(h, w, y, x);
        new       = newpan(win[BORD]); 
        hide_panel(new); // No peeking

        win[BODY] = derwin(win[BORD], hshrink(h), hshrink(w), 1, 1);
        wcolor_set(win[BODY], PUR_PUR, NULL);
        for (i=0; i<w; i++)        
                mvwaddwstr(win[BORD], h-1, i, L"█"); /* Draw bottom */

        keypad(win[BODY], 1);
        wbkgrnd(win[BORD], FILL);
        wbkgrnd(win[BODY], FILL);
        vrt_refresh();

        return (new);
}

// Menus
//##############################################################################

// 00. THE DOCK BOX
WINDOW *dockbox_win;
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
                dockbox_win = newwin(1, COLS, LINES-1, 0);
                dockbox_buf = derwin(dockbox_win, 1, COLS-30, 0, 30);
                wbkgrnd(dockbox_win, &PURPLE[2]);
                wbkgrnd(dockbox_buf, &PURPLE[2]);
                dockbox_pan = newpan(dockbox_win);
        }
}


// 00. THE CREW MENU
#define CREW_W 20

WINDOW *crew_win[WPARTS];
MENU   *crew_menu;

void post_menu_crew(void)
{
        int i;
        int n = npersons();
        char **name;
        ITEM **item;

        if (crew_pan == NULL)
                crew_pan = purple_panel(crew_win, thH, CREW_W+5, thB, 3);

        /* Collect the menu items. */
        name = calloc(n, sizeof(char *));

        for (i=0; i<n; i++) {
                name[i] = flname(person_list[i]);
        }

        /* Build the menu. */
        item = calloc(n+1, sizeof(ITEM *));

        for (i=0; i<n; i++) {
                item[i] = new_item(name[i], name[i]);
                set_item_userptr(item[i], &person_list[i]);
        }
        item[n] = (ITEM *)NULL;

        crew_menu = new_menu((ITEM **)item);

        /* Configure and post the menu */
        set_menu_win(crew_menu, crew_win[BORD]);
        set_menu_sub(crew_menu, crew_win[BODY]);
        set_menu_fore(crew_menu, COLOR_PAIR(PUR_GRE));
        set_menu_back(crew_menu, COLOR_PAIR(PUR_PUR));
        menu_opts_off(crew_menu, O_SHOWDESC);
        set_menu_format(crew_menu, thH-3, 1);
        set_menu_mark(crew_menu, "");

        post_menu(crew_menu);

}



enum operator_modes { 
        EXITING,

        FULLNAME, 
        FIRSTLASTNAME, 
        FIRSTNAME, 
        MIDDLENAME, 
        LASTNAME,
                      
        PROFESSION, 
        GUILD,
                      
        PHYSICAL, 
        GENDER,
        AGE, 
        WEIGHT, 
        HEIGHT,

        ATTRIBUTES,
        SKILL,
        STRENGTH,
        DEXTERITY,
        VITALITY,
        AGILITY,
        INTELLIGENCE,
        WISDOM,
        CHARISMA,
        LUCK
};
const char *mode_tag[] = { 
        "",
        "Full name: ",
        "Name: ",
        "First name: ",
        "Middle name: ",
        "Last name: ",
        "Job: ",
        "Guild: ",
        "Physical: ",
        "Sex: ",
        "Age: ",
        "Weight: ",
        "Height: ",
        "Attributes: ",
        "Skill: ",
        "Strength: ",
        "Dexterity: ",
        "Vitality: ",
        "Agility: ",
        "Intelligence: ",
        "Wisdom: ",
        "Charisma: ",
        "Luck: "
};


inline void allattr(WINDOW *win, int ofs, uint32_t key)
{
        #define STRIDE 5

        int val[NATTRS];
        int i;

        attr_all(val, key);
        for (i=0; i<NATTRS; i++) {
                mvwprintw(win, 0, ofs+(i*STRIDE)+1, "%02u", val[i]);
        }
}


#define NMODES 23
                      
int mode;
int mode_changed;

#define RESET -1 

inline void setmode(int newmode)
{
        if (mode == newmode) return;
        if (newmode == RESET) mode_changed ^= 1;
        else {
                mode = newmode;
                mode_changed = 1;
        }
}


void operate_on(void *noun)
{
        #define _out_ dockbox_buf
        #define NORMAL_COLOR PUR_PUR
        #define SELECT_COLOR PUR_YEL

        static int label_width;
        static char *label;
        static int a[8];
       
        if (mode_changed) {
                werase(_out_);
                if (mode == EXITING) return;
                else {
                        label = mode_tag[mode];
                        label_width = strlen(label);
                        werase(_out_);
                        wcolor_set(_out_, NORMAL_COLOR, NULL);
                        wprintw(_out_, "%s", label);
                        wcolor_set(_out_, SELECT_COLOR, NULL);

                        win_refresh(_out_);
                        setmode(RESET);
                }
        }

        wmove(_out_, 0, label_width);
        wclrtoeol(_out_);

        switch(mode)
        {
        case ATTRIBUTES:
                wcolor_set(_out_, PUR_GRE, NULL);
                waddwstr(_out_, L"Σ    Φ    Δ    A    Ψ    W    Χ    Λ");
                wcolor_set(_out_, NORMAL_COLOR, NULL);
                allattr(_out_, label_width, *(uint32_t *)noun);
                win_refresh(_out_);
                break;
        case FULLNAME:
                wprintw(_out_, "%s", fullname(*(uint32_t *)noun));
                win_refresh(_out_);
                break;
        case FIRSTLASTNAME:
                wprintw(_out_, "%s", flname(*(uint32_t *)noun));
                win_refresh(_out_);
                break;
        case FIRSTNAME:
                wprintw(_out_, "%s", fname(*(uint32_t *)noun));
                win_refresh(_out_);
                break;
        case MIDDLENAME:
                wprintw(_out_, "%s", mname(*(uint32_t *)noun));
                win_refresh(_out_);
                break;
        case LASTNAME:
                wprintw(_out_, "%s", lname(*(uint32_t *)noun));
                win_refresh(_out_);
                break;
        case PROFESSION:
                wprintw(_out_, "%s", getjob(*(uint32_t *)noun));
                win_refresh(_out_);
                break;
        }
}


void choose_noun(void) 
{
        ITEM *item = NULL;
        int c;

        while ((c = getch())) 
        {
                switch (c)
                {
                case 'k':
                        menu_driver(crew_menu, REQ_PREV_ITEM);
                        break;
                case 'j':
                        menu_driver(crew_menu, REQ_NEXT_ITEM);
                        break;
                case 'n':
                        timeout(1000);
                        switch(getch()) 
                        {
                        case 'N':
                                setmode(FULLNAME);
                                break;
                        case 'f':
                                setmode(FIRSTNAME);
                                break;
                        case 'm':
                                setmode(MIDDLENAME);
                                break;
                        case 'l':
                                setmode(LASTNAME);
                                break;
                        }
                        timeout(-1);
                        break;
                case 'p':
                        setmode(PROFESSION);
                        break;
                case 's':
                        setmode(ATTRIBUTES);
                        break;
                case 'o':
                        setmode(EXITING);
                        operate_on(NULL);

                        TOGPAN(crew_pan);
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
        TOGPAN(crew_pan);
        scr_refresh();
        choose_noun();
}
