#pragma once
#ifndef __INVENTORY_MENU_H
#define __INVENTORY_MENU_H

struct item_t *equipped;

#define EAT_ITEM 3333

WINDOW *equipped_win;
PANEL  *equipped_pan;

void inventory_mkmenu(struct list_head *inv);
void inventory_to_menu(struct list_head *inv);
int inventory_menu_control(int input);
struct item_t *invmenu_curitem(void);
struct item_t *invmenu_previtem(void);

#endif
