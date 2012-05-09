#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdmenu.h"
#include "../lib/hash.h"
#include "../lib/stoc/stoc.h"
#include "../equip/items.h"
#include "../equip/rope.h"
#include "inventory.h"


void inventory_get(uint32_t key, struct mob_t *mob);
void inventory_add(struct equip_t *equip, struct mob_t *mob);
void inventory_use(uint32_t key, struct mob_t *mob);
void inventory_burn(uint32_t key, struct mob_t *mob);
void inventory_mkmenu(struct mob_t *mob);


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

        return (new);
}


/* Inventory methods
``````````````````````````````````````````````````````````````````````````````*/
/* 
 * inventory_get -- set the 'tmp' ptr to the hashed object stored at 'key'
 */
void inventory_get(uint32_t key, struct mob_t *mob)
{
        mob->inv->tmp = (struct equip_t *)hashtable_get(mob->inv->tbl, key);
}

/* 
 * inventory_add -- add a new piece of equipment to the inventory hash table 
 */
void inventory_add(struct equip_t *equip, struct mob_t *mob)
{
        hashtable_add(mob->inv->tbl, equip->id, equip);
        mob->inv->key[mob->inv->n++] = equip->id;
}

/* 
 * inventory_use -- retreive the hashed object and call its 'use' method
 */
void inventory_use(uint32_t key, struct mob_t *mob)
{
        mob->inv->get(key, mob);
        mob->inv->tmp->use(mob, mob->inv->tmp);
}

/* 
 * inventory_burn -- retreive the hashed object and call its 'burn' method
 */
void inventory_burn(uint32_t key, struct mob_t *mob)
{
        mob->inv->get(key, mob);
        if (mob->inv->tmp && mob->inv->tmp->burn)
                mob->inv->tmp->burn(mob, mob->inv->tmp);
}

/*
 * inventory_mkmenu -- generate an inventory menu
 */
void inventory_mkmenu(struct mob_t *mob)
{
        #define MENU_W 16
        #define MENU_H (LINES/6)
        #define MENU_X 0 
        #define MENU_Y (LINES-MENU_H)
        #define ORIGINAL PUR_GRE
        #define STANDOUT WARNING
        #define OTANDOUT PUR_RED
        char     **name;
        wchar_t  **icon;
        uint32_t **id;
        int i;
        int n;

        mob->inv->add(new_equipment(ITEM_ROPE), mob);
        mob->inv->add(new_equipment(ITEM_PICKAXE), mob);
        mob->inv->add(new_equipment(ITEM_TORCH), mob);

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

        mob->inv->menu = new_stdmenu(name, name, icon, (void **)id, n);
        mob->inv->menu->nitem = n;

        stdmenu_win(mob->inv->menu, MENU_H, MENU_W, MENU_Y, MENU_X, 1, 0, 3, 0);
        stdmenu_color_item(mob->inv->menu, STANDOUT, ORIGINAL, __PUR_GREY);
        stdmenu_color_name(mob->inv->menu, PUR_YEL, ORIGINAL, __PUR_GREY);
        stdmenu_cfg(mob->inv->menu, DESC, false, NULL);
        stdmenu_cfg(mob->inv->menu, MARK, false, NULL);

        mob->inv->menu->post(mob->inv->menu);
}


WINDOW *lh_win;
WINDOW *rh_win;
PANEL *lh_pan;
PANEL *rh_pan;

void print_hands(struct mob_t *mob)
{
        if (!lh_win || !rh_win) {
                lh_win = newwin(1, 3, LINES-1, 0);
                lh_pan = new_panel(lh_win);
                rh_win = newwin(1, 3, LINES-1, 3);
                rh_pan = new_panel(rh_win);
        }

        if (mob->inv->lh) {
                mob->inv->get(mob->inv->lh, mob); 
                mvwadd_wch(dock_window(DOCK_WIN), 0, 1, 
                           mkcch(mob->inv->tmp->wch, 0, mob->inv->tmp->pair));
        }
        if (mob->inv->rh) {
                mob->inv->get(mob->inv->rh, mob); 
                mvwadd_wch(dock_window(DOCK_WIN), 0, 4, 
                           mkcch(mob->inv->tmp->wch, 0, mob->inv->tmp->pair));
        }
}


void inventory_control(struct mob_t *mob, int input)
{
        #define INV_MENU(mob) ((mob)->inv->menu)
        static bool done;

        if (INV_MENU(mob) == NULL)
                mob->inv->mkmenu(mob);

        switch (input) {

        /* Enter noun menu mode --------------------------- */
        case MODE_STARTED:
                break;

        /* Menu navigation -------------------------------- */
        case 'k':
                INV_MENU(mob)->prev(INV_MENU(mob));
                break;
        case 'j':
                INV_MENU(mob)->next(INV_MENU(mob));
                break;
        case 'K':
                INV_MENU(mob)->pgup(INV_MENU(mob));
                break;
        case 'J':
                INV_MENU(mob)->pgdn(INV_MENU(mob));
                break;


        /* Select / open the subject or object menu ------- */
        case 'i':
                INV_MENU(mob)->tog(INV_MENU(mob));
                break;
        case 'e':
        case 'l':
                mob->inv->lh = *((uint32_t *)INV_MENU(mob)->cur_ptr);
                done = true;
                break;
        case 'r':
                mob->inv->rh = *((uint32_t *)INV_MENU(mob)->cur_ptr);
                done = true;
                break;

        /* Exit noun menu mode ---------------------------- */
        case KEY_ESC:
        case 'm':
        finished:
                done = false;
                INV_MENU(mob)->unfocus(INV_MENU(mob));
                INV_MENU(mob)->close(INV_MENU(mob));
                return MODE_RESTORE;
        }

        print_hands(mob);
        INV_MENU(mob)->icons(INV_MENU(mob), 1, 1);

        if (done)
                goto finished;
        
        scr_refresh();
        return MODE_PERSIST;
}

