#pragma once
#ifndef __UI_DOCK_H
#define __UI_DOCK_H


enum dockw_enum {DOCK_WIN, NAME_WIN, STAT_WIN, TEXT_WIN};


WINDOW *dock_win(int windowid);
void dock_toggle(void);
void dock_update(void);


void tab_cur(int tab);
void tab_sig(int tab);
void tab_tog(int tab);
void tab_update(void);


#endif
