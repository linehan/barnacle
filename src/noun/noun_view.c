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
        #define STANDOUT PUR_DYE
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


struct stdmenu_t *get_noun_struct(int op)
{
        return nounmenu[op];
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



/*
 * print_current_noun -- print the name of the currently selected noun
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void print_current_noun(int op)
{
        if (op > 1) return;
        werase(IDWIN(op));

        if (op==SUBJECT) {
                wcolor_set(IDWIN(op), PUR_YEL, NULL);
                wprintw(IDWIN(op), "%-*s", ID_W,  fullname(request_id(op)));
        }
        if (op==OBJECT) {
                wcolor_set(IDWIN(op), PUR_YEL, NULL);
                wprintw(IDWIN(op), "%*s", ID_W, fullname(request_id(op)));
        }
        win_refresh(IDWIN(op));
}


/*
 * grey_current_noun -- grey out the name of the currently selected noun
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void grey_current_noun(int op)
{
        if (op > 1) return;
        werase(IDWIN(op));
        if (op==SUBJECT) {
                wcolor_set(IDWIN(op), PUR_GRE, NULL);
                wprintw(IDWIN(op), "%-*s", ID_W, fullname(request_id(op)));
        }
        if (op==OBJECT) {
                wcolor_set(IDWIN(op), PUR_GRE, NULL);
                wprintw(IDWIN(op), "%*s", ID_W, fullname(request_id(op)));
        }
        win_refresh(IDWIN(op));
}



/* -------------------------------------------------------------------------- */
/* It's widgets the whole way down */
/* -------------------------------------------------------------------------- */



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                            vitality widget                                 //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//           dark   medium  light   filled   large vert.    large hoz.        //
//              \     |     /         |         |              /              //
enum box_styles{DBOX,MBOX,LBOX,XBOX,FBOX,EBOX,LGVBAR,SMVBAR,LGHBAR,SMHBAR};   //
//                              /         |            /              \       //
//                        checkerboard   empty  small vert.        small hoz. //
////////////////////////////////////////////////////////////////////////////////
static const short   VIT_PAIR[]={PUR_RED, PUR_BRZ, PUR_BLU, PUR_GRE};
static const char   *VIT_TAGS[]={   "HP",    "SP",    "LP",    "EP"};
static const wchar_t  BOX_WCH[]=L"▩▨▧▦▣□ⲾⲿⲺⲻ";
static cchar_t BOX_CCH;


inline void put_nblocks(WINDOW *win, int y, int x, int block, short pair, int n)
{
        setcchar(&BOX_CCH, &BOX_WCH[block], 0, pair, NULL);
        mvwhline_set(win, y, x, &BOX_CCH, n);
}


void view_vitals(int op)
{
        if (op!=SUBJECT && op!=OBJECT) op=SUBJECT;

        #define X(op) (op==SUBJECT) ? 0 : VIT_MAXLEN

        int vital;      // value of each vital in loop
        int i, xofs;

        werase(WIWIN(op));
        put_nblocks(WIWIN(op), 0, 0, SMHBAR, TITLE_SCREEN, VIT_MAXLEN);

        key_noun(request_id(op));
        xofs = X(op);

        for (i=0; i<4; i++) {
                vital = get_vital(request_id(op), i);
                if (op==OBJECT)  xofs -= vital;
                put_nblocks(WIWIN(op), 0, xofs, DBOX, VIT_PAIR[i], vital);
                if (op==SUBJECT) xofs += vital;
        }
        win_refresh(WIWIN(op));
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                           attributes widget                                //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void view_attributes(int op)
{
        #define STRIDE 5
        #define ATTR_STANDOUT PUR_GRE
        #define ATTR_ORIGINAL PUR_PUR

        int val[8];
        int i;

        werase(WIWIN(op));
        wcolor_set(WIWIN(op), ATTR_STANDOUT, NULL);
        waddwstr(WIWIN(op), L"Σ    Φ    Δ    A    Ψ    W    Χ    Λ");
        wcolor_set(WIWIN(op), ATTR_ORIGINAL, NULL);

        unpack_attributes(request_id(SUBJECT), val);
        for (i=0; i<8; i++) {
                mvwprintw(WIWIN(op), 0, (i*STRIDE)+1, "%02u", val[i]);
        }
        win_refresh(WIWIN(op));
}

