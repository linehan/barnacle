#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdmenu.h"
#include "../lib/hash.h"
#include "../lib/stoc/stoc.h"
#include "../equip/equipment.h"
#include "../equip/rope.h"
#include "inventory.h"


void inventory_get(void *self, uint32_t key);
void inventory_add(void *self, struct equip_t *equip);
void inventory_use(void *self, uint32_t key);
void inventory_burn(void *self, uint32_t key);
void inventory_mkmenu(void *self);


#define MENU_W 12
#define MENU_H (LINES/6)
#define MENU_X 1 
#define MENU_Y (LINES-MENU_H)

/*
 * new_inventory -- create and initialize an inventory structure
 */
struct inventory_t *new_inventory(struct noun_t *noun)
{
        struct inventory_t *new = calloc(1, sizeof(struct inventory_t));

        new->tbl = new_hashtable(0);
        new->noun = noun;

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
void inventory_get(void *self, uint32_t key)
{
        struct inventory_t *inv = (struct inventory_t *)self; 

        inv->tmp = (struct equip_t *)hashtable_get(inv->tbl, key);
}


/* inventory_add -- add a new piece of equipment to the inventory hash table */
void inventory_add(void *self, struct equip_t *equip)
{
        struct inventory_t *inv = (struct inventory_t *)self; 

        hashtable_add(inv->tbl, equip->id, equip);
        
        inv->key[inv->n++] = equip->id;

        if (equip->tag == ITEM_TORCH)
                inv->torch_key = equip->id;
        if (equip->tag == ITEM_ROPE)
                inv->rope_key = equip->id;
}


/* inventory_use -- retreive the hashed object and call its 'use' method */
void inventory_use(void *self, uint32_t key)
{
        struct inventory_t *inv = (struct inventory_t *)self; 

        inv->get(inv, key);
        inv->tmp->use(inv->tmp, inv->noun);
}

/* inventory_burn -- retreive the hashed object and call its 'burn' method */
void inventory_burn(void *self, uint32_t key)
{
        struct inventory_t *inv = (struct inventory_t *)self; 

        inv->get(inv, key);
        if (inv->tmp && inv->tmp->burn)
                inv->tmp->burn(inv->tmp, inv->noun);
}


/* inventory_mkmenu -- generate an inventory menu */
void inventory_mkmenu(void *self)
{
        #define ORIGINAL PUR_GRE
        #define STANDOUT WARNING
        #define OTANDOUT PUR_RED
        char     **name;
        wchar_t  **icon;
        uint32_t **id;
        int i;
        int n;

        struct inventory_t *inv = (struct inventory_t *)self;

        inv->add(inv, new_equipment(ITEM_ROPE));
        inv->add(inv, new_equipment(ITEM_PICKAXE));
        inv->add(inv, new_equipment(ITEM_SHOVEL));
        inv->add(inv, new_equipment(ITEM_TORCH));

        name = calloc(inv->n, sizeof(char *));      /* Item names */
        id   = calloc(inv->n, sizeof(uint32_t *));  /* Item hash ids */
        icon = calloc(inv->n, sizeof(wchar_t *));

        for (i=0; i<(inv->n); i++) {
                inv->get(inv, inv->key[i]);
                name[i] = inv->tmp->name;
                icon[i] = inv->tmp->wch;
                id[i]   = &inv->tmp->id;
        }

        inv->menu = new_stdmenu(name, name, icon, (void **)id, inv->n);
        inv->menu->nitem = inv->n;

        stdmenu_win(inv->menu, MENU_H, MENU_W, MENU_Y, MENU_X, 1, 0, 3, 0);
        stdmenu_color_item(inv->menu, STANDOUT, ORIGINAL, __PUR_GREY);
        stdmenu_color_name(inv->menu, PUR_YEL, ORIGINAL, __PUR_GREY);
        stdmenu_cfg(inv->menu, DESC, false, NULL);
        stdmenu_cfg(inv->menu, MARK, false, NULL);

        inv->menu->post(inv->menu);
        inv->menu->next(inv->menu);
        inv->menu->prev(inv->menu);
}


static inline void print_item(struct inventory_t *inv, short pair)
{
        wcolor_set(inv->equipped_win, pair, NULL);
        mvwadd_wch(inv->equipped_win, 0, 1, mkcch(inv->tmp->wch, 0, pair));
        mvwprintw(inv->equipped_win, 0, 3, "%s\n", inv->tmp->name);
        inv->menu->icons(inv->menu, 1, 1);
}

static inline void grab_current(struct inventory_t *inv)
{
        uint32_t *keyptr = (uint32_t *)inv->menu->cur_ptr;
        CUR_KEY(inv) = *keyptr;
        inv->get(inv, CUR_KEY(inv));
}


enum mobmode { INVENTORY_START, INVENTORY_EXIT, INVENTORY_DEFAULT };

static inline int operate_on(struct inventory_t *inv, int mode)
{
        int return_value;

        grab_current(inv);

        switch (mode) {
        case INVENTORY_START:
        case INVENTORY_DEFAULT:
                print_item(inv, PUR_GREY);
                return_value = MODE_PERSIST;
                break;

        case INVENTORY_EXIT:
                print_item(inv, PUR_PURPLE);
                inv->menu->close(inv->menu);
                return_value = MODE_RESTORE;
                break;
        }

        scr_refresh();
        return return_value;
}


void inventory_control(struct inventory_t *inv, int input)
{
        int mode = INVENTORY_DEFAULT;

        if (inv->menu == NULL)
                inv->mkmenu(inv);

        switch (input) {

        case MODE_STARTED:
                mode = INVENTORY_START;
                break;
        case 'k':
                inv->menu->prev(inv->menu);
                break;
        case 'j':
                inv->menu->next(inv->menu);
                break;
        case 'K':
                inv->menu->pgup(inv->menu);
                break;
        case 'J':
                inv->menu->pgdn(inv->menu);
                break;
        case 'i':
                inv->menu->tog(inv->menu);
                break;
        case KEY_ESC:
        case 'm':
        case ' ':
                mode = INVENTORY_EXIT;
                break;
        }

        return operate_on(inv, mode);
}

