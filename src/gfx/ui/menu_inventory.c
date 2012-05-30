/* 
 * menu_inventory.c -- Inventory menu manipulation and rendering.
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "../../com/barnacle.h"
#include "stdmenu.h"
#include "../../item/item.h"
#include "menu_inventory.h"

struct stdmenu_t *invmenu; /* The inventory menu */
struct item_t *equipped;   /* Currently equipped item */

/* Equipped item gets its own panel */
WINDOW *equipped_win;
PANEL  *equipped_pan;

typedef struct item_t INVENTORY_ITEM;

#define MENU_W 15
#define MENU_H (LINES/6)
#define MENU_X 1 
#define MENU_Y (LINES-MENU_H)


static inline void print_item(struct item_t *item, short pair)
{
        top_panel(equipped_pan);
        wcolor_set(equipped_win, pair, NULL);
        mvwcch(equipped_win, 0, 1, item->icon, 0, pair);
        mvwprintw(equipped_win, 0, 3, "%s\n", item->name);
        invmenu->icons(invmenu, 1, 1);
}

inline void add_test_items(struct list_head *inv)
{
        INVENTORY_ITEM *test[3];
        int i;

        test[0] = make_item(ITEM_ROPE);
        test[1] = make_item(ITEM_PICKAXE);
        test[2] = make_item(ITEM_TORCH);

        for (i=0; i<3; i++)
                list_add(inv, &test[i]->node);
}

/*
 * SETUP
 * Create the windows and panels and set the backgrounds
 */
inline void inventory_setup(void)
{
        equipped_win = newwin(1, MENU_W, LINES-1, 1);
        wbkgrnd(equipped_win, &PURPLE[2]);
        equipped_pan = new_panel(equipped_win);
}


/**
 * inventory_to_menu -- generate an inventory menu from a list of items 
 * @inv: linked list of items (inventory)
 */
void inventory_to_menu(struct list_head *inv)
{
        INVENTORY_ITEM **item;
        INVENTORY_ITEM *tmp;
        char     **name;
        wchar_t  **icon;
        int i;
        int n=0;

        if (list_empty(inv))
                return;

        list_count(n, inv, tmp, node); 

        name = calloc(n, sizeof(char *));      /* Item names */
        icon = calloc(n, sizeof(wchar_t *));
        item = calloc(n, sizeof(struct item_t *));

        i=0;
        list_for_each(inv, tmp, node) {
                name[i] = tmp->name;
                icon[i] = tmp->icon;
                item[i] = tmp;
                i++;
        }

        if (!equipped_win) {
                inventory_setup();
                invmenu = new_stdmenu(name, name, icon, (void **)item, n);
        } else {
                invmenu->unpost(invmenu);
                invmenu->build(invmenu, name, name, icon, (void **)item, n);
                invmenu->post(invmenu);
                invmenu->next(invmenu);
                invmenu->prev(invmenu);
                invmenu->icons(invmenu, 1, 1);
        }
}





/* 
 * CONSTRUCTOR
 * inventory_mkmenu -- generate an inventory menu for the first time
 */ 
void inventory_mkmenu(struct list_head *inv)
{
        #define ORIGINAL ____PUR_GREY
        #define STANDOUT WARNING
        #define OTANDOUT PUR_RED

        add_test_items(inv);

        inventory_to_menu(inv);

        stdmenu_win(invmenu, MENU_H, MENU_W, MENU_Y, MENU_X, 1, 0, 3, 0);
        stdmenu_color_item(invmenu, STANDOUT, ORIGINAL, ____PUR_PURPLE);
        stdmenu_color_name(invmenu, PUR_YEL, ORIGINAL, ____PUR_PURPLE);
        stdmenu_cfg(invmenu, DESC, false, NULL);
        stdmenu_cfg(invmenu, MARK, false, NULL);

        invmenu->post(invmenu);
        invmenu->next(invmenu);
        invmenu->prev(invmenu);
}


struct item_t *invmenu_curitem(void)
{
        return (struct item_t *)invmenu->cur_ptr;
}

struct item_t *invmenu_previtem(void)
{
        return (struct item_t *)invmenu->old_ptr;
}




enum mobmode { INVENTORY_START, INVENTORY_EXIT, INVENTORY_DEFAULT, INVENTORY_USE };
bool print_description;



void inventory_seek(int n)
{
        int last = -1;
        int seek;
                
        invmenu->first(invmenu);

        while (seek=invmenu->cursor, (seek != n) && (seek != last)) {
                last = seek; 
                invmenu->next(invmenu);
        }

}


static inline int operate_on(void *current, int mode)
{
        int return_value;

        ITEM_OBJECT(item, current);

        switch (mode) {
        case INVENTORY_START:
        case INVENTORY_DEFAULT:
                print_item(item, PUR_GREY);
                return_value = MODE_PERSIST;
                if (print_description)
                        say_speak(L"", item->desc);
                break;

        case INVENTORY_EXIT:
                print_item(item, PUR_PURPLE);
                print_description = false;
                say_speak(L"", "");
                invmenu->close(invmenu);
                return_value = MODE_RESTORE;
                break;
        case INVENTORY_USE:
                item->use(item, PLAYER);
                return_value = MODE_PERSIST;
                break;
        }

        equipped = current;

        scr_refresh();
        return return_value;
}

struct item_t *oldie;
int nnn;

int inventory_menu_control(int input)
{
        int mode = INVENTORY_DEFAULT;

        assert(invmenu || !"Inventory menu has not been created");

        switch (input) {

        case MODE_STARTED:
                mode = INVENTORY_START;
                break;
        case 'k':
                invmenu->prev(invmenu);
                break;
        case 'j':
                invmenu->next(invmenu);
                break;
        case 'K':
                invmenu->pgup(invmenu);
                break;
        case 'J':
                invmenu->pgdn(invmenu);
                break;
        case 'i':
                invmenu->tog(invmenu);
                break;
        case 'l':
                print_description = true;
                break;
        case 'h':
                if (print_description) {
                        print_description = false;
                        say_speak(L"", "");
                }
                break;
        case EAT_ITEM:
                nnn = invmenu->cursor;
                PLAYER->eat(PLAYER);
                inventory_seek(nnn);
                break;
        case ' ':
                mode = INVENTORY_USE;
                break;
        case KEY_ESC:
        case 'm':
                mode = INVENTORY_EXIT;
                break;
        }
        return operate_on(invmenu->cur_ptr, mode);
}


void update_inventory_menu(void)
{
        if (equipped_pan)
                top_panel(equipped_pan);
}

