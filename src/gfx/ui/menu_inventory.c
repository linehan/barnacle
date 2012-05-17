#include "../../com/arawak.h"
#include "stdmenu.h"
#include "../../item/item.h"


struct stdmenu_t *invmenu;
struct item_t *equipped;
WINDOW *equipped_win;
PANEL  *equipped_pan;


#define MENU_W 12
#define MENU_H (LINES/6)
#define MENU_X 1 
#define MENU_Y (LINES-MENU_H)


inline void add_test_items(struct list_head *inv)
{
        int i;
        struct item_t *test[4];

        test[0] = make_item(ITEM_ROPE);
        test[1] = make_item(ITEM_PICKAXE);
        test[2] = make_item(ITEM_SHOVEL);
        test[3] = make_item(ITEM_TORCH);

        for (i=0; i<4; i++)
                list_add(inv, &test[i]->node);
}


/*
 * SETUP
 * Create the windows and panels and set the backgrounds
 */
inline void inventory_setup(void)
{
        equipped_win = newwin(1, 12, LINES-1, 1);
        wbkgrnd(equipped_win, &PURPLE[2]);
        equipped_pan = new_panel(equipped_win);
}


/* 
 * CONSTRUCTOR
 * inventory_mkmenu -- generate an inventory menu 
 */ 
void inventory_mkmenu(struct list_head *inv)
{
        #define ORIGINAL PUR_GRE
        #define STANDOUT WARNING
        #define OTANDOUT PUR_RED
        char     **name;
        wchar_t  **icon;
        struct item_t **item;
        struct item_t *tmp;
        int i=0;
        int n=0;

        if (list_empty(inv))
                add_test_items(inv);

        if (!equipped_win)
                inventory_setup();

        list_for_each(inv, tmp, node)
                n++;

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

        invmenu = new_stdmenu(name, name, icon, (void **)item, n);

        stdmenu_win(invmenu, MENU_H, MENU_W, MENU_Y, MENU_X, 1, 0, 3, 0);
        stdmenu_color_item(invmenu, STANDOUT, ORIGINAL, __PUR_GREY);
        stdmenu_color_name(invmenu, PUR_YEL, ORIGINAL, __PUR_GREY);
        stdmenu_cfg(invmenu, DESC, false, NULL);
        stdmenu_cfg(invmenu, MARK, false, NULL);

        invmenu->post(invmenu);
        invmenu->next(invmenu);
        invmenu->prev(invmenu);
}



static inline void print_item(struct item_t *item, short pair)
{
        top_panel(equipped_pan);
        wcolor_set(equipped_win, pair, NULL);
        mvwadd_wch(equipped_win, 0, 1, mkcch(item->icon, 0, pair));
        mvwprintw(equipped_win, 0, 3, "%s\n", item->name);
        invmenu->icons(invmenu, 1, 1);
}



enum mobmode { INVENTORY_START, INVENTORY_EXIT, INVENTORY_DEFAULT };



static inline int operate_on(void *current, int mode)
{
        int return_value;

        ITEM_OBJECT(item, current);

        switch (mode) {
        case INVENTORY_START:
        case INVENTORY_DEFAULT:
                print_item(item, PUR_GREY);
                return_value = MODE_PERSIST;
                break;

        case INVENTORY_EXIT:
                print_item(item, PUR_PURPLE);
                invmenu->close(invmenu);
                return_value = MODE_RESTORE;
                break;
        }

        equipped = current;

        scr_refresh();
        return return_value;
}


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
        case KEY_ESC:
        case 'm':
        case ' ':
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

