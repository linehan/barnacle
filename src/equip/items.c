#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdmenu.h"
#include "../lib/hash.h"

struct hashtable_t *inventory_tbl;

struct equip_t {
        wchar_t *wch;
        short pair;
        char *name;
        char *desc;
        uint32_t id;
};

struct equip_t stuff[]={
        {L"Ꮻ", PUR_PURPLE, "Sleepbag", ""},
        {L"⚸", PUR_PURPLE, "Machete", ""},
        {L"⚗", PUR_PURPLE, "Alembic", ""},
        {L"ཞ", PUR_PURPLE, "Skull", ""},
        {L"ໃ", PUR_PURPLE, "Plumline",""},
        {L"Ⴃ", PUR_PURPLE, "Potion", ""},
        {L"ⴃ", PUR_PURPLE, "Draught", ""},
        {L"Ꭵ", PUR_PURPLE, "Candlestick", ""},
        {L"ང", PUR_PURPLE, "Hammer", ""},
        {L"⍑", PUR_PURPLE, "Mallet", ""},
        {L"ⴶ", PUR_PURPLE, "Maul", ""},
        {L"⌛", PUR_PURPLE, "Hourglass", ""},
        {L"ⅴ", PUR_PURPLE, "Goblet", ""},
        {L"∢", PUR_PURPLE, "Sextant", ""},
        {L"⊾", PUR_PURPLE, "Square", ""},
        {L"ᐰ", PUR_PURPLE, "Compass", ""},
        {L"⊿", PUR_PURPLE, "Angle", ""},
        {L"⫿", PUR_PURPLE, "Rule", ""},
        {L"∣", PUR_PURPLE, "Slide", ""},
        {L"⊍", PUR_PURPLE, "Beaker", ""},
        {L"Ⴄ", PUR_PURPLE, "Hatchet", ""},
        {L"Ĳ", PUR_PURPLE, "Map", ""},
        {L"℧", PUR_PURPLE, "Bottle", ""},
        {L"ᘮ", PUR_PURPLE, "Pot", ""},
        {L"Ỽ", PUR_PURPLE, "Retort", ""},
        {L"ރ", PUR_PURPLE, "Cross", ""},
        {L"∮", PUR_PURPLE, "Grain", ""},
        {L"∯", PUR_PURPLE, "Grain", ""},
        {L"∰", PUR_PURPLE, "Grain", ""},
        {L"Ⳛ", PUR_PURPLE, "Bellows", ""},
        {L"⳨", PUR_PURPLE, "Rapier", ""},
        {L"⚷", PUR_PURPLE, "Elixir", ""},
        {L"Ⱂ", PUR_PURPLE, "Tomahawk", ""},
        {L"⸙", PUR_PURPLE, "Frond", ""},
        {L"ⴡ", PUR_PURPLE, "Pick", ""},
        {L"Ⳓ", PUR_PURPLE, "Rope", ""},
        {L"ở", PUR_PURPLE, "Gas", ""},
        {L"ޖ", PUR_PURPLE, "Hook", ""},
        {L"⧸", PUR_PURPLE, "Rod", ""},
        {L"ᗻ", PUR_PURPLE, "Folio", ""},
        {L"⥽", PUR_PURPLE, "Grapple", ""},
        {L"⦆", PUR_PURPLE, "Shortbow", ""},
        {L"⦈", PUR_PURPLE, "Longbow", ""},
        {L"⦘", PUR_PURPLE, "Fatbow", ""},
        {L"⦄", PUR_PURPLE, "Techbow", ""},
        {0},

// Compass ⊾∡∢ᗅᗉᗆ
// Hooks ɂ⥼⥽
// Containers ꝺꝹởỏỼỽ
// Books ᗼᗻᗻᗼ
// Sleeping bag Ꮻ
// ᝢ
// ⦅⦆⦇⦈⦉⦊⦗⦘⧘⧙⎰⎱
// ∮∯∰∱∲∳
// ∾∿≀
// ߞ߀ߌ
// ⬫߀⬨⬯

};

WINDOW *lh_win;
WINDOW *rh_win;
PANEL *lh_pan;
PANEL *rh_pan;
static uint32_t lh_key;
static uint32_t rh_key;


/* -------------------------------------------------------------------------- */
#define MENU_W 16
#define MENU_H (LINES/6)
#define MENU_X 0 
#define MENU_Y (LINES-MENU_H)

#define ORIGINAL PUR_GRE
#define STANDOUT WARNING
#define OTANDOUT PUR_RED
/* -------------------------------------------------------------------------- */

struct stdmenu_t *inventory;

/**
 * list_nouns -- generate the nouns list based queries and put them in a menu
 * @query: the query, an optional filter that filters the nouns in some way
 */
struct stdmenu_t *make_inventory(int h, int w, int y, int x)
{
        struct stdmenu_t *new;
        uint32_t **id;
        char **name;
        wchar_t **icon;
        int i;
        int n;

        if (!inventory_tbl)
                inventory_tbl = new_hashtable(0);

        for (n=0; stuff[n].name != NULL; n++) {
                stuff[n].id = fasthash(stuff[n].name, strlen(stuff[n].name));
                hashtable_add(inventory_tbl, stuff[n].id, &stuff[n]);
        }

        name = calloc(n, sizeof(char *));      /* Item names */
        id   = calloc(n, sizeof(uint32_t *));  /* Item hash ids */
        icon = calloc(n, sizeof(wchar_t *));

        i = n;
        while (i-->0) {
                name[i] = stuff[i].name;
                icon[i] = stuff[i].wch;
                id[i]   = &stuff[i].id;
        }

        new = new_stdmenu(name, name, icon, (void **)id, n);
        new->nitem = n;

        stdmenu_win(new, h, w, y, x, 1, 0, 3, 0);
        stdmenu_color_item(new, STANDOUT, ORIGINAL, __PUR_GREY);
        stdmenu_color_name(new, PUR_YEL, ORIGINAL, __PUR_GREY);
        stdmenu_cfg(new, DESC, false, NULL);
        stdmenu_cfg(new, MARK, false, NULL);

        new->post(new);

        return (new);
}


void print_name(struct stdmenu_t *smenu)
{
        struct equip_t *itm; /* Inventory item object */
        uint32_t *key;       /* Receives hash keys from menu user pointer */ 

        key  = (uint32_t *)smenu->cur_ptr;
        itm  = (struct equip_t *)(hashtable_get(inventory_tbl, *key));

        mvwprintw(dock_window(STAT_WIN), 0, 0, "%s\n", itm->name);
        scr_refresh();
}

void print_hands(struct stdmenu_t *stdmenu)
{
        struct equip_t *lh;
        struct equip_t *rh;

        if (!lh_win || !rh_win) {
                lh_win = newwin(1, 3, LINES-1, 0);
                lh_pan = new_panel(lh_win);
                rh_win = newwin(1, 3, LINES-1, 3);
                rh_pan = new_panel(rh_win);
        }

        if (lh_key) {
                lh = (struct equip_t *)hashtable_get(inventory_tbl, lh_key);
                mvwadd_wch(dock_window(DOCK_WIN), 0, 1, mkcch(lh->wch, 0, lh->pair));
        }
        if (rh_key) { 
                rh = (struct equip_t *)hashtable_get(inventory_tbl, rh_key);
                mvwadd_wch(dock_window(DOCK_WIN), 0, 3, mkcch(rh->wch, 0, rh->pair));
        }
}

void inventory_control(int input)
{
        struct equip_t *tmp;
        static bool done;

        if (!inventory)
                inventory = make_inventory(MENU_H, MENU_W, MENU_Y, MENU_X);

        switch (input) {

        /* Enter noun menu mode --------------------------- */
        case MODE_STARTED:
                break;

        /* Menu navigation -------------------------------- */
        case 'k':
                inventory->prev(inventory);
                break;
        case 'j':
                inventory->next(inventory);
                break;
        case 'K':
                inventory->pgup(inventory);
                break;
        case 'J':
                inventory->pgdn(inventory);
                break;


        /* Select / open the subject or object menu ------- */
        case 'i':
                inventory->tog(inventory);
                break;
        case 'e':
        case 'l':
                lh_key = *((uint32_t *)inventory->cur_ptr);
                done = true;
                break;
        case 'r':
                rh_key = *((uint32_t *)inventory->cur_ptr);
                break;

        /* Exit noun menu mode ---------------------------- */
        case KEY_ESC:
        case 'm':
        finished:
                done = false;
                inventory->unfocus(inventory);
                inventory->close(inventory);
                return MODE_RESTORE;
        }

        print_hands(inventory);
        inventory->icons(inventory, 1, 1);

        if (done)
                goto finished;
        
        scr_refresh();
        return MODE_PERSIST;
}

