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
#include "noun_model.h"
#include "noun_view.h"
#include  "noun_widgets.h"

/* -------------------------------------------------------------------------- */

#define MENU_W 21
#define MENU_H (LINES/3)
#define MENU_X(op) (op == SUBJECT) ? 1 : COLS-(MENU_W+1)
#define MENU_Y LINES-MENU_H-2

#define NUM_NOUNMENUS 2

struct stdmenu_t *nounmenu[NUM_NOUNMENUS];

#define IDWIN(op) (op == SUBJECT) ? (dock_window(SUBJ_ID_WIN)) : (dock_window(OBJ_ID_WIN))
#define WIWIN(op) (op == SUBJECT) ? (dock_window(SUBJ_WI_WIN)) : (dock_window(OBJ_WI_WIN))

/*
 * list_nouns -- generate the nouns list based queries and put them in a menu
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 * @query: the query, an optional filter that filters the nouns in some way
 */
void list_nouns(int op, int query)
{
        #define ORIGINAL PUR_GRE
        /*#define STANDOUT PUR_DYE*/
        #define STANDOUT WARNING
        #define OTANDOUT PUR_RED

        uint32_t **key;
        char **name;
        int nitems;
        int i;

        nitems = numnoun;


        name = calloc(nitems, sizeof(char *));      /* Noun names */
        key  = calloc(nitems, sizeof(uint32_t *));  /* Noun ids */

        for (i=0; i<nitems; i++) {
                name[i] = fullname(keyring[i]);
                key[i]  = &keyring[i];
        }

        nounmenu[op] = new_stdmenu(name, name, (void **)key, nitems);

        nounmenu[op]->nitems = nitems;

        stdmenu_win(nounmenu[op], MENU_H, MENU_W, MENU_Y, MENU_X(op));
        stdmenu_buf(nounmenu[op], IDWIN(op));

        wcolor_set(nounmenu[op]->buf, PUR_YEL, NULL);

        stdmenu_color(nounmenu[op], STANDOUT, ORIGINAL, __PUR_GREY);
        stdmenu_cfg(nounmenu[op], DESC, false, NULL);
        /*stdmenu_cfg(nounmenu[op], MARK, false, NULL);*/

        nounmenu[op]->post(nounmenu[op], true);
}


/*
 * focus_noun_menu -- give a visual hint that the cursor's focus has changed
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void focus_noun_menu(int op, bool opt)
{
        if (opt == false)
                stdmenu_color(nounmenu[op], PUR_WHITE, ORIGINAL, __PUR_GREY);
        else
                stdmenu_color(nounmenu[op], STANDOUT, ORIGINAL, __PUR_GREY);
}

/*
 * Return the stdmenu struct of the operand
 */
struct stdmenu_t *get_noun_struct(int op)
{
        return nounmenu[op];
}

/*
 * Return a pointer to the struct noun_t of the currently selected noun
 * in the menu of the operand.
 */
struct noun_t *current_noun(int op)
{
        ITEM *item;
        uint32_t key;

        item = current_item(nounmenu[op]->menu);
        key  = *((uint32_t *)item_userptr(item));
        key_noun(key);

        return focused;
}



/*
 * get_noun_menu -- return a pointer to the passed operand's MENU
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
MENU *get_noun_menu(int op)
{
        if (nounmenu[op] == NULL)
                list_nouns(op, ALL_NOUNS);

        return nounmenu[op]->menu;
}

/*
 * isvis_noun_menu -- returns true if operand is visible, else returns false
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
bool isvis_noun_menu(int op)
{
        return (nounmenu[op]->isvis(nounmenu[op])) ? true : false;
}

/*
 * open_noun_menu -- call the noun menu's 'vis' method to open the menu
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void open_noun_menu(int op)
{
        if (nounmenu[op] == NULL)
                list_nouns(op, ALL_NOUNS);
        else
                nounmenu[op]->vis(nounmenu[op], true);

        vrt_refresh();
        scr_refresh();
}


/*
 * close_noun_menu -- call the noun menu's 'vis' method to close the menu
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void close_noun_menu(int op)
{
        if (nounmenu[op] == NULL)
                list_nouns(op, ALL_NOUNS);
        else
                nounmenu[op]->vis(nounmenu[op], false);

        scr_refresh();
}


/*
 * tog_noun_menu -- call the noun menu's 'vis' method to toggle the menu
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void tog_noun_menu(int op)
{
        if (panel_hidden(nounmenu[op]->pan))
                nounmenu[op]->vis(nounmenu[op], true);
        else
                nounmenu[op]->vis(nounmenu[op], false);
}


