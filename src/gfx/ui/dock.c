#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <menu.h>
#include "../gfx.h"
#include "../../noun/noun_model.h"
#include "../../noun/noun_view.h"
#include "dock.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                dock window                                 //
//                                                                            //
//                                                                            //
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

void init_dock(void)
{
        dock_win = newwin(DOCK_H, DOCK_W, LINES-2, DOCK_X);

        subject_win = derwin(   dock_win , OPERAND_H , OPERAND_W+20 , 0        , SUBJECT_X);
        subj_id_win = derwin(subject_win , ID_H      , ID_W      , ID_Y     , 0);
        subj_wi_win = derwin(subject_win , WIDGET_H  , WIDGET_W  , WIDGET_Y , 0);
        subj_tx_win = derwin(subject_win , ID_H      , ID_W      , ID_Y+1   , 40);

        object_win  = derwin(  dock_win  , OPERAND_H , OPERAND_W , 0        , OBJECT_X);
        obj_id_win  = derwin(object_win  , ID_H      , ID_W      , ID_Y     , OPERAND_W-ID_W-2);
        obj_wi_win  = derwin(object_win  , WIDGET_H  , WIDGET_W  , WIDGET_Y , OPERAND_W-WIDGET_W-2);
        obj_tx_win  = derwin(object_win  , ID_H      , ID_W      , ID_Y     , OPERAND_W-WIDGET_W-32);

        wbkgrnd(dock_win, &PURPLE[2]);
        wbkgrnd(subject_win, &PURPLE[2]);
        wbkgrnd(subj_id_win, &PURPLE[2]);
        wbkgrnd(subj_wi_win, &PURPLE[2]);
        wbkgrnd(subj_tx_win, &PURPLE[2]);
        wbkgrnd(object_win, &PURPLE[2]);
        wbkgrnd(obj_id_win, &PURPLE[2]);
        /*wbkgrnd(obj_wi_win, &PURPLE[2]);*/
        /*wbkgrnd(obj_tx_win, &PURPLE[2]);*/
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
        if (dock_pan == NULL) init_dock();
        if (get_noun_menu(0) == NULL || get_noun_menu(1) == NULL)
                list_nouns(SUBJECT, ALL_NOUNS);
        /*if (get_noun_menu(1) == NULL) */
                /*list_nouns(OBJECT, ALL_NOUNS);*/
        else    
                top_panel(dock_pan);
}





