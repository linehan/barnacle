/* 
 * noun_menus.c 
 *
 * Provides a set of handles for creating and querying the two main menus
 * that display the set of loaded nouns, or a subset based on the application
 * of a filter (query).
 *
 */

#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdpan.h"
#include "../gfx/ui/stdmenu.h"
#include "../gfx/ui/dock.h"
#include "../test/test.h"
#include "noun.h"

/* -------------------------------------------------------------------------- */
#define MENU_W 21
#define MENU_H (LINES/3)
#define MENU_X 1
#define MENU_Y (LINES-MENU_H-1)

#define IDWIN dock_window(NAME_WIN)
#define WIWIN dock_window(STAT_WIN)

#define ORIGINAL PUR_GRE
#define STANDOUT WARNING
#define OTANDOUT PUR_RED
/* -------------------------------------------------------------------------- */


struct stdmenu_t *nounmenu; /* Global noun menu */


/* Helpers and accessors 
``````````````````````````````````````````````````````````````````````````````*/
/* get_nounmenu -- return a pointer to the passed operand's MENU */
MENU *get_nounmenu(void)
{
        return nounmenu->menu;
}
/* get_noun_struct -- return ptr to the nounmenu */
struct stdmenu_t *get_noun_struct(void)
{
        return nounmenu;
}
/* current_noun -- return ptr to the currently selected noun */
struct noun_t *current_noun(void)
{
        /* What hath God wrought */
        key_noun(*((uint32_t *)item_userptr(current_item(nounmenu->menu))));
        return focused;
}


/* Noun menu constructor 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * list_nouns -- generate the nouns list based queries and put them in a menu
 * @query: the query, an optional filter that filters the nouns in some way
 */
void make_nounmenu(int query)
{
        uint32_t **key;
        char **name;
        int nitem;
        int i;

        nitem = numnoun;

        name = calloc(nitem, sizeof(char *));      /* Noun names */
        key  = calloc(nitem, sizeof(uint32_t *));  /* Noun ids */

        for (i=0; i<nitem; i++) {
                name[i] = fullname(keyring[i]);
                key[i]  = &keyring[i];
        }

        nounmenu = new_stdmenu(name, name, NULL, (void **)key, nitem);
        nounmenu->nitem = nitem;

        stdmenu_win(nounmenu, MENU_H, MENU_W, MENU_Y, MENU_X, 1, 1, 1, 1);
        stdmenu_buf(nounmenu, IDWIN);
        stdmenu_color_item(nounmenu, STANDOUT, ORIGINAL, __PUR_GREY);
        stdmenu_color_name(nounmenu, PUR_YEL, ORIGINAL, __PUR_GREY);
        stdmenu_cfg(nounmenu, DESC, false, NULL);

        nounmenu->post(nounmenu);
}


/* Noun menu widgets and rendering 
``````````````````````````````````````````````````````````````````````````````*/
/*
 * nounmenu_print_highlight -- apply a highlight attribute to the operand
 */
void nounmenu_print_highlight(void)
{
        struct noun_t *noun;
        attr_t attr;
        short pair;

        noun = current_noun();
        wattr_get(mob_win(&noun->mob), &attr, &pair, NULL);

        /* 
         * ncurses doesn't really provide an easy way to compare
         * attr_t, which is just a typedef'd int. If you don't
         * believe me, change the if statement to test whether
         * attr == WA_REVERSE, and see what happens. 
         */
        if ((attr & WA_REVERSE) == WA_REVERSE) {
                wattr_off(mob_win(&noun->mob), WA_REVERSE, NULL);
                wchgat(mob_win(&noun->mob), -1, WA_NORMAL, pair, NULL);
        } else {
                wattr_on(mob_win(&noun->mob), WA_REVERSE, NULL);
                wchgat(mob_win(&noun->mob), -1, WA_REVERSE, pair, NULL);
        }
}

/*
 * nounmenu_print_name -- print the name of the currently selected noun
 * @pair: the color pair of the printed text
 */
void nounmenu_print_name(short pair)
{
        static const int width = 20;

        werase(IDWIN);
        wcolor_set(IDWIN, pair, NULL);

        wprintw(IDWIN, "%-*s", width,  fullname(request_id(SUBJECT)));

        win_refresh(IDWIN);
}

/**
 * put_nblocks -- draw some boxes... this is pretty old code and should
 * probably be factored out 

            dark   med  light check  fill empty lg & sm vert. lg & sm hoz.        
                \    |    \     |     |    /    /     \       /     \         */
enum box_styles{DBOX,MBOX,LBOX,XBOX,FBOX,EBOX,LGVBAR,SMVBAR,LGHBAR,SMHBAR}; 

static const short   VIT_PAIR[]={PUR_RED, PUR_BRONZE, PUR_BLU, PUR_GRE};
static const char   *VIT_TAGS[]={   "HP",    "SP",    "LP",    "EP"};
static const wchar_t  BOX_WCH[]=L"▩▨▧▦▣□ⲾⲿⲺⲻ";

static cchar_t BOX_CCH;

inline void put_nblocks(WINDOW *win, int y, int x, int block, short pair, int n)
{
        setcchar(&BOX_CCH, &BOX_WCH[block], 0, pair, NULL);
        mvwhline_set(win, y, x, &BOX_CCH, n);
}

/**
 * nounmenu_print_vitals -- draw the vitals bar from the noun's vitals word 
 */
void nounmenu_print_vitals(void)
{
        int vital; /* Value of each vital byte */ 
        int xofs;  /* Offset for put_nblocks */
        int i;

        xofs = 0;

        werase(WIWIN);
        put_nblocks(WIWIN, 0, xofs, SMHBAR, TITLE_SCREEN, VIT_MAXLEN);

        key_noun(request_id(SUBJECT));

        for (i=0; i<4; i++) {
                vital = get_vital(request_id(SUBJECT), i);
                put_nblocks(WIWIN, 0, xofs, DBOX, VIT_PAIR[i], vital);
                xofs += vital;
        }
        win_refresh(WIWIN);
}

/**
 * nounmenu_print_attributes -- draw the attributes with some fanciness
 */
void nounmenu_print_attributes(void)
{
        #define ATTR_STANDOUT PUR_GRE
        #define ATTR_ORIGINAL PUR_PUR

        const int STRIDE=5;
        int val[8];
        int i;

        werase(WIWIN);
        wcolor_set(WIWIN, ATTR_STANDOUT, NULL);
        waddwstr(WIWIN, L"Σ    Φ    Δ    A    Ψ    W    Χ    Λ");
        wcolor_set(WIWIN, ATTR_ORIGINAL, NULL);

        unpack_attributes(request_id(SUBJECT), val);
        for (i=0; i<8; i++) {
                mvwprintw(WIWIN, 0, (i*STRIDE)+1, "%02u", val[i]);
        }
        win_refresh(WIWIN);
}

