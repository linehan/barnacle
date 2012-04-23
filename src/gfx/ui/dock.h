#pragma once
#ifndef __UI_DOCK_H
#define __UI_DOCK_H

enum dockw_enum { DOCK_WIN, SUBJECT_WIN, SUBJ_ID_WIN, SUBJ_WI_WIN,
                            OBJECT_WIN,  OBJ_ID_WIN,  OBJ_WI_WIN, SUBJ_TX_WIN, OBJ_TX_WIN };

#define DOCK_H 3
#define DOCK_W COLS
#define DOCK_Y LINES-DOCK_H
#define DOCK_X 0

#define OPERAND_H 3
#define OPERAND_W 50
#define ID_H 1
#define ID_W 20
#define ID_Y 2 
#define WIDGET_H 1
#define WIDGET_W 32
#define WIDGET_Y 1

#define SUBJECT_X 2
#define OBJECT_X COLS-OPERAND_W

void init_dock(void);
WINDOW *dock_window(int windowid);
void view_dock(void);

void hide_dock(void);


void do_tab_sig(void);
void tab_sig(int tab);

#endif
