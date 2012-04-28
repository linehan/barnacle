#include "../../com/arawak.h"
#include "../gfx.h"
#include "../../noun/noun_model.h"
#include "../../noun/noun_view.h"
#include "dock.h"


/* Tab icons, notices, and selectors
``````````````````````````````````````````````````````````````````````````````*/
/*
  +-------------------------------------------------------------------------+
  |                                                                         |
  | [][][][][][][][][][]                                    +---------------+
  | Beefalo Jonathan                                        | % | $ | @ | & |
  +---------------------------------------------------------+---------------+ 
                                                            \**** These ****/
                                                           
#define NUMTABS 4 


struct ui_tab_t {
        WINDOW *win;    /* Parent window */
        WINDOW *ico;    /* Holds the tab's icon */
        WINDOW *cur;    /* Holds the cursor (below the tab) */
        PANEL  *pan;
        bool sigtrue;   /* TRUE if tab is being signalled */
        bool curtrue;   /* TRUE if tab has cursor below it */
};

struct ui_tab_t ui_tab[NUMTABS];


#define TAB_H    2
#define TAB_W    3
#define TAB_Y    (LINES-(TAB_H))
#define TAB_X(i) (COLS-(NUMTABS*TAB_W)+(TAB_W*i))


static const wchar_t *cursor_wch   = L"๏";
static const short cursor_color    = PUR_PURPLE;
static const short signal_color[2] = {PUR_PURPLE, PUR_GREY};
static cchar_t cursor_cch;


/*
 * Initialize and draw the tabs in ui_tab[]
 */
void init_tabs(void)
{
        const wchar_t *wch[NUMTABS] = { L"⸙", L"⬎", L"ℜ", L"⸭" };
        const short colors[NUMTABS];
        static cchar_t cch[NUMTABS];
        int i;

        setcchar(&cursor_cch, cursor_wch, 0, cursor_color, NULL);

        for (i=0; i<NUMTABS; i++) {
                /* Create the window and subwindow */
                ui_tab[i].win = newwin(TAB_H, TAB_W, TAB_Y, TAB_X(i));
                ui_tab[i].ico = derwin(ui_tab[i].win, 1, 1, 0, 0);
                ui_tab[i].cur = derwin(ui_tab[i].win, 1, 1, 1, 0);

                /* Paint the icon character */
                setcchar(&cch[i], wch[i], 0, PUR_PURPLE, NULL);

                /* Draw the icon in the subwindow */
                wadd_wch(ui_tab[i].ico, &cch[i]);

                /* Draw the background color */
                wbkgrnd(ui_tab[i].win, &PURPLE[2]);
                wbkgrnd(ui_tab[i].ico, &PURPLE[2]);
                wbkgrnd(ui_tab[i].cur, &PURPLE[2]);

                /* Create the panel */ 
                ui_tab[i].pan = new_panel(ui_tab[i].win);
        }
}

/*
 * tab_sig -- toggle the signal status of a tab
 * @tab: index of the tab to toggle
 */
void tab_sig(int tab)
{
        ui_tab[tab].sigtrue ^= true;
}

/*
 * tab_cur -- control the placement of the tab cursor
 * @tab: index of the tab to toggle, *or* 'l'/'r' for next/prev tab
 */
void tab_cur(int tab)
{
        static uint8_t current;

        werase(ui_tab[current].cur); /* Erase old one */

        switch (tab) {
        case 'l':
                current = (current > 0) ? (current-1) : (NUMTABS-1);
                break;
        case 'r':
                current = (current < NUMTABS-1) ? (current+1) : (0);
                break;
        default:
                current = tab % NUMTABS;
                break;
        }

        wadd_wch(ui_tab[current].cur, &cursor_cch); /* Write new one */
}

/*
 * tab_update -- update the rendering of every tab
 * 
 * Notes
 * This is a hook for the event loop to call in order to keep the tabs
 * updated. Its primary use at the moment is to make the signal highlight
 * "blink" on and off during alternating calls.
 */
void tab_update(void)
{
        static uint8_t cycle; 
        int i;

        cycle ^= 1;

        for (i=0; i<NUMTABS; i++) {
                if (ui_tab[i].sigtrue) {
                        wchgat(ui_tab[i].ico, 1, 0, signal_color[cycle], NULL);
                }
        }
}



/* The dock container 
````````````````````````````````````````````````````````````````````````````````
  +-------------------------------------------------------------------------+
  |                                                                         |
  | [][][][][][][][][][]                                    +---------------+
  | Beefalo Jonathan                                        | % | $ | @ | & |
  +---------------------------------------------------------+---------------+ */

struct ui_dock_t {
        WINDOW *dock_win;       /* The mother of all windows */
        WINDOW *name_win;       /* "Beefalo Jonathan" */
        WINDOW *stat_win;       /* All the [][][]'s */
        WINDOW *text_win;       /* Not yet implemented */
        PANEL  *dock_pan;
        PANEL  *name_pan;
        PANEL  *stat_pan;
        PANEL  *text_pan;
        bool is_visible;        /* Used by dock_toggle */
};


struct ui_dock_t ui_dock; /* The one and only dock */


/*
 * Create the dock
 */
void init_dock(void)
{
        #define DOCK_HEIGHT 3
        #define DOCK_WIDTH COLS
        #define DOCK_Y LINES-DOCK_HEIGHT
        #define DOCK_X 0

        #define NAME_HEIGHT 1
        #define NAME_WIDTH 20
        #define NAME_Y LINES - NAME_HEIGHT
        #define NAME_X 2
        
        #define STAT_HEIGHT 1
        #define STAT_WIDTH 32
        #define STAT_Y LINES - 2 
        #define STAT_X 2

        #define TEXT_HEIGHT 2 
        #define TEXT_WIDTH 32
        #define TEXT_Y LINES - 2 
        #define TEXT_X COLS/2 

        init_tabs();

        ui_dock.dock_win = newwin(DOCK_HEIGHT, DOCK_WIDTH, DOCK_Y, DOCK_X);
        ui_dock.name_win = newwin(NAME_HEIGHT, NAME_WIDTH, NAME_Y, NAME_X);
        ui_dock.stat_win = newwin(STAT_HEIGHT, STAT_WIDTH, STAT_Y, STAT_X);
        ui_dock.text_win = newwin(TEXT_HEIGHT, TEXT_WIDTH, TEXT_Y, TEXT_X);

        wbkgrnd(ui_dock.dock_win, &PURPLE[2]);
        wbkgrnd(ui_dock.name_win, &PURPLE[2]);
        wbkgrnd(ui_dock.stat_win, &PURPLE[2]);
        wbkgrnd(ui_dock.text_win, &PURPLE[2]);

        ui_dock.dock_pan = new_panel(ui_dock.dock_win);
        ui_dock.name_pan = new_panel(ui_dock.name_win);
        ui_dock.stat_pan = new_panel(ui_dock.stat_win);
        ui_dock.text_pan = new_panel(ui_dock.text_win);
}


/*
 * Return a particular window of the dock structure (used by the noun
 * and verb view routines)
 */
WINDOW *dock_window(int windowid)
{
        WINDOW *win[]={
                ui_dock.dock_win, 
                ui_dock.name_win, 
                ui_dock.stat_win,
                ui_dock.text_win
        };

        if (windowid > 3) return NULL;
        else         return (win[windowid]);
}


/*
 * Quick check to make sure that the panels to be shown or hidden
 * actually exist.
 */
inline void check_for_init(void)
{
        if (!ui_dock.dock_pan) 
                init_dock();
        if (get_noun_menu(0) == NULL || get_noun_menu(1) == NULL)
                list_nouns(SUBJECT, ALL_NOUNS);
}

/*
 * Implementation note
 *
 * UI elements like this dock are interfaced through 4 primary functions.
 *
 *      1. view_*: Makes the UI element visible on-screen 
 *      2. hide_*: Hides the UI element so it is not rendered on-screen 
 *      3. *_toggle: Calls either view_* or hide_*, depending on a boolean
 *      4. *_update: For the event loop to call; keeps elements updated
 *
 * The first two are for internal use within this module. The second pair
 * are admissible interfaces: *_toggle is used by the FSM (user input), 
 * and *_update is used by the event loop.
 *
 */

/*
 * view_dock -- make the dock panels visible at the bottom of the screen
 */
void view_dock(void)
{
        int i;

        check_for_init();
        ui_dock.is_visible = true;

        show_panel(ui_dock.dock_pan);
        show_panel(ui_dock.name_pan);
        show_panel(ui_dock.stat_pan);
        show_panel(ui_dock.text_pan);

        for (i=0; i<NUMTABS; i++) 
                show_panel(ui_tab[i].pan);

        scr_refresh();
}

/*
 * hide_dock -- hide the dock panels so they are no longer visible
 */
void hide_dock(void)
{
        int i;

        check_for_init();
        ui_dock.is_visible = false;

        hide_panel(ui_dock.dock_pan);
        hide_panel(ui_dock.name_pan);
        hide_panel(ui_dock.stat_pan);
        hide_panel(ui_dock.text_pan);

        for (i=0; i<NUMTABS; i++)
                hide_panel(ui_tab[i].pan);

        scr_refresh();
}

/*
 * dock_toggle -- toggle the dock view between hidden and visible
 */
void dock_toggle(void) 
{
        (ui_dock.is_visible) ? hide_dock() : view_dock();
}

/*
 * dock_update -- hook for the event loop to keep the dock rendering 
 */
void dock_update(void)
{
        if (!ui_dock.is_visible)
                return;

        view_dock();
        /*do_pulse();*/
}



/* Special junk
````````````````````````````````````````````````````````````````````````````` */
void dock_say(const wchar_t *who, const char *msg)
{
        werase(ui_dock.text_win);

        mvwadd_wch(ui_dock.text_win, 0, 0, mkcch(who, 0, PUR_YELLOW));
        wcolor_set(ui_dock.text_win, PUR_YELLOW, NULL);
        mvwprintw(ui_dock.text_win, 0, 3, "%s\n", msg);

        wrefresh(ui_dock.text_win);
}



