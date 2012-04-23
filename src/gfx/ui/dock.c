#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <menu.h>
#include "../gfx.h"
#include "../../noun/noun_model.h"
#include "../../noun/noun_view.h"
#include "dock.h"
/*//////////////////////////////////////////////////////////////////////////////
+------------------------------------------------------------------------------+
|                               dock parent window                             |
+------------------------------------------------------------------------------+
+-----------------------------------+      +-----------------------------------+
|          subject window           |      |           object window           |
+-----------------------------------+      +-----------------------------------+
+-----------+ +---------------------+      +---------------------+ +-----------+
| id window | |    widget window    |      |    widget window    | | id window |
+-----------+ +---------------------+      +---------------------+ +-----------+
*///////////////////////////////////////////////////////////////////////////////
WINDOW *dock_win;
WINDOW *subject_win, *subj_id_win, *subj_wi_win, *subj_tx_win;
WINDOW *object_win, *obj_id_win, *obj_wi_win, *obj_tx_win;
PANEL  *dock_pan;


/* Tab icons, notices, and selectors
``````````````````````````````````````````````````````````````````````````````*/

#define NUMTABS 4 
#define TAB_H 2
#define TAB_W 3
#define TAB_Y (LINES-(TAB_H))
#define TAB_X(i) (COLS-TAB_W-(TAB_W*i))

struct ui_tab_t {
        WINDOW *win;    /* Hold subwindows */
        WINDOW *ico;  
        WINDOW *cur;
        PANEL  *pan;
        bool sigtrue;
        bool curtrue;
};

struct ui_tab_t ui_tab[NUMTABS];

static const wchar_t *cursor_wch = L"๏";
static const short cursor_color  = PUR_PURPLE;
static const short signal_color[2] = {PUR_PURPLE, PUR_GREY};
static cchar_t cursor_cch;


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

void tab_sig(int tab)
{
        ui_tab[tab].sigtrue ^= true;
}

void tab_cur(int tab)
{
        static uint8_t current;

        werase(ui_tab[current].cur); /* Erase old one */
        current = tab;

        wadd_wch(ui_tab[current].cur, &cursor_cch); 
}

void draw_tabs(void)
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



void init_dock(void)
{
        init_tabs();

        dock_win = newwin(DOCK_H, DOCK_W, LINES-DOCK_H, DOCK_X);

        subject_win = derwin(   dock_win , OPERAND_H , OPERAND_W+20 , 0        , SUBJECT_X);
        subj_id_win = derwin(subject_win , ID_H      , ID_W      , ID_Y     , 0);
        subj_wi_win = derwin(subject_win , WIDGET_H  , WIDGET_W  , WIDGET_Y , 0);
        subj_tx_win = derwin(subject_win , ID_H      , ID_W      , ID_Y+1   , 40);

        /*object_win  = derwin(  dock_win  , OPERAND_H , OPERAND_W , 0        , OBJECT_X);*/
        /*obj_id_win  = derwin(object_win  , ID_H      , ID_W      , ID_Y     , OPERAND_W-ID_W-2);*/
        /*obj_wi_win  = derwin(object_win  , WIDGET_H  , WIDGET_W  , WIDGET_Y , OPERAND_W-WIDGET_W-2);*/
        /*obj_tx_win  = derwin(object_win  , ID_H      , ID_W      , ID_Y     , OPERAND_W-WIDGET_W-32);*/

        wbkgrnd(dock_win, &PURPLE[2]);
        wbkgrnd(subject_win, &PURPLE[2]);
        wbkgrnd(subj_id_win, &PURPLE[2]);
        wbkgrnd(subj_wi_win, &PURPLE[2]);
        wbkgrnd(subj_tx_win, &PURPLE[2]);
        /*wbkgrnd(object_win, &PURPLE[2]);*/
        /*wbkgrnd(obj_id_win, &PURPLE[2]);*/
        dock_pan = new_panel(dock_win);
}


WINDOW *dock_window(int windowid)
{
        WINDOW *win[]={dock_win, subject_win, subj_id_win, subj_wi_win,
                       object_win, obj_id_win, obj_wi_win, subj_tx_win, obj_tx_win };

        if (windowid > 8) return NULL;
        else         return (win[windowid]);
}



void view_dock(void)
{
        int i;

        if (dock_pan == NULL) init_dock();
        if (get_noun_menu(0) == NULL || get_noun_menu(1) == NULL)
                list_nouns(SUBJECT, ALL_NOUNS);
        else {
                top_panel(dock_pan);
                for (i=0; i<NUMTABS; i++) {
                        top_panel(ui_tab[i].pan);
                }
        }
}


void hide_dock(void)
{
        int i;
        if (dock_pan == NULL) 
                init_dock();
        if (get_noun_menu(0) == NULL || get_noun_menu(1) == NULL)
                list_nouns(SUBJECT, ALL_NOUNS);

        for (i=0; i<NUMTABS; i++) {
                hide_panel(ui_tab[i].pan);
        }
        hide_panel(dock_pan);
}

