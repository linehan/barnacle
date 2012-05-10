#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdmenu.h"
#include "../lib/hash.h"
#include "../lib/stoc/stoc.h"
#include "../equip/equipment.h"
#include "../equip/rope.h"
#include "inventory.h"


void inventory_get(uint32_t key, struct mob_t *mob);
void inventory_add(struct equip_t *equip, struct mob_t *mob);
void inventory_use(uint32_t key, struct mob_t *mob);
void inventory_burn(uint32_t key, struct mob_t *mob);
void inventory_mkmenu(struct mob_t *mob);


#define MENU_W 12
#define MENU_H (LINES/6)
#define MENU_X 1 
#define MENU_Y (LINES-MENU_H)

/*
 * new_inventory -- create and initialize an inventory structure
 */
struct inventory_t *new_inventory(void)
{
        struct inventory_t *new = calloc(1, sizeof(struct inventory_t));

        new->tbl = new_hashtable(0);

        new->use    = &inventory_use;
        new->add    = &inventory_add;
        new->get    = &inventory_get;
        new->burn   = &inventory_burn;
        new->mkmenu = &inventory_mkmenu;

        new->equipped_win = newwin(1, 12, LINES-1, 1);
        wbkgrnd(new->equipped_win, &PURPLE[2]);
        new->equipped_pan = new_panel(new->equipped_win);

        return (new);
}


/* Inventory methods
``````````````````````````````````````````````````````````````````````````````*/
/* inventory_get -- set the 'tmp' ptr to the hashed object stored at 'key' */
void inventory_get(uint32_t key, struct mob_t *mob)
{
        mob->inv->tmp = (struct equip_t *)hashtable_get(mob->inv->tbl, key);
}

/* inventory_add -- add a new piece of equipment to the inventory hash table */
void inventory_add(struct equip_t *equip, struct mob_t *mob)
{
        hashtable_add(mob->inv->tbl, equip->id, equip);
        mob->inv->key[mob->inv->n++] = equip->id;

        if (equip->tag == ITEM_TORCH)
                mob->inv->torch_key = equip->id;
        if (equip->tag == ITEM_ROPE)
                mob->inv->rope_key = equip->id;
}

/* inventory_use -- retreive the hashed object and call its 'use' method */
void inventory_use(uint32_t key, struct mob_t *mob)
{
        mob->inv->get(key, mob);
        mob->inv->tmp->use(mob, mob->inv->tmp);
}

/* inventory_burn -- retreive the hashed object and call its 'burn' method */
void inventory_burn(uint32_t key, struct mob_t *mob)
{
        mob->inv->get(key, mob);
        if (mob->inv->tmp && mob->inv->tmp->burn)
                mob->inv->tmp->burn(mob, mob->inv->tmp);
}

/* inventory_mkmenu -- generate an inventory menu */
void inventory_mkmenu(struct mob_t *mob)
{
        #define ORIGINAL PUR_GRE
        #define STANDOUT WARNING
        #define OTANDOUT PUR_RED
        char     **name;
        wchar_t  **icon;
        uint32_t **id;
        int i;
        int n;

        inv_add(new_equipment(ITEM_ROPE), mob);
        inv_add(new_equipment(ITEM_PICKAXE), mob);
        inv_add(new_equipment(ITEM_SHOVEL), mob);
        inv_add(new_equipment(ITEM_TORCH), mob);

        n = mob->inv->n;

        name = calloc(n, sizeof(char *));      /* Item names */
        id   = calloc(n, sizeof(uint32_t *));  /* Item hash ids */
        icon = calloc(n, sizeof(wchar_t *));

        for (i=0; i<(n); i++) {
                mob->inv->get(mob->inv->key[i], mob);
                name[i] = mob->inv->tmp->name;
                icon[i] = mob->inv->tmp->wch;
                id[i]   = &mob->inv->tmp->id;
        }

        inv_menu(mob) = new_stdmenu(name, name, icon, (void **)id, n);
        inv_menu(mob)->nitem = n;

        stdmenu_win(inv_menu(mob), MENU_H, MENU_W, MENU_Y, MENU_X, 1, 0, 3, 0);
        stdmenu_color_item(inv_menu(mob), STANDOUT, ORIGINAL, __PUR_GREY);
        stdmenu_color_name(inv_menu(mob), PUR_YEL, ORIGINAL, __PUR_GREY);
        stdmenu_cfg(inv_menu(mob), DESC, false, NULL);
        stdmenu_cfg(inv_menu(mob), MARK, false, NULL);

        inv_menu(mob)->post(inv_menu(mob));
        inv_menu(mob)->next(inv_menu(mob));
        inv_menu(mob)->prev(inv_menu(mob));
}


static inline void print_item(struct mob_t *mob, short pair)
{
        wcolor_set(inv_eqwin(mob), pair, NULL);
        mvwadd_wch(inv_eqwin(mob), 0, 1, mkcch(inv_tmp(mob)->wch, 0, pair));
        mvwprintw(inv_eqwin(mob), 0, 3, "%s\n", inv_tmp(mob)->name);
        inv_menu(mob)->icons(inv_menu(mob), 1, 1);
}

static inline void grab_current(struct mob_t *mob)
{
        uint32_t *keyptr = (uint32_t *)inv_menu(mob)->cur_ptr;
        CUR_KEY(mob) = *keyptr;
        inv_get(CUR_KEY(mob), mob);
}


enum mobmode { INVENTORY_START, INVENTORY_EXIT, INVENTORY_DEFAULT };

static inline int operate_on(int mode, struct mob_t *mob)
{
        int return_value;

        grab_current(mob);

        switch (mode) {
        case INVENTORY_START:
        case INVENTORY_DEFAULT:
                print_item(mob, PUR_GREY);
                return_value = MODE_PERSIST;
                break;

        case INVENTORY_EXIT:
                print_item(mob, PUR_PURPLE);
                inv_menu(mob)->close(inv_menu(mob));
                return_value = MODE_RESTORE;
                break;
        }

        scr_refresh();
        return return_value;
}


void inventory_control(struct mob_t *mob, int input)
{
        int mode = INVENTORY_DEFAULT;

        if (inv_menu(mob) == NULL)
                inv_mkmenu(mob);

        switch (input) {

        case MODE_STARTED:
                mode = INVENTORY_START;
                break;
        case 'k':
                inv_menu(mob)->prev(inv_menu(mob));
                break;
        case 'j':
                inv_menu(mob)->next(inv_menu(mob));
                break;
        case 'K':
                inv_menu(mob)->pgup(inv_menu(mob));
                break;
        case 'J':
                inv_menu(mob)->pgdn(inv_menu(mob));
                break;
        case 'i':
                inv_menu(mob)->tog(inv_menu(mob));
                break;
        case KEY_ESC:
        case 'm':
        case ' ':
                mode = INVENTORY_EXIT;
                break;
        }

        return operate_on(mode, mob);
}

