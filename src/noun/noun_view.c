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
 * sort_noun_menu -- sort the noun menu items according to some criteria
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void sort_noun_menu(int op, uint32_t sort)
{
        ITEM **item;    /* All the items in the menu */
        ITEM *tmp;      /* For swapping items during sort */
        int nitems;     /* Number of items in total */
        int i;
        int j;

        nitems = nounmenu[op]->nitems;
        item   = nounmenu[op]->item;
        tmp    = NULL;

        for (i=0; i<nitems; i++) {
                if (item_opts(item[i]) == O_SELECTABLE) {
                        tmp = item[i];
                        /* Swap match with first non-selectable item */
                        for (j=0; j<nitems; j++) {
                                if (item_opts(item[j]) != O_SELECTABLE)
                                        break; 
                        } 
                        item[i] = item[j];
                        item[j] = tmp;
                }
        }
        unpost_menu(nounmenu[op]->menu);
        set_menu_items(nounmenu[op]->menu, nounmenu[op]->item);
        post_menu(nounmenu[op]->menu);
        scr_refresh();
}


/*
 * query_noun_menu -- list nearby nouns in order of increasing distance
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void query_noun_menu(int op, uint32_t query)
{
        ITEM **item;    /* All the items in the menu */
        int nitems;     /* Number of items in total */
        int i;

        nitems = nounmenu[op]->nitems;
        item   = nounmenu[op]->item;

        /* 
         * Any nouns with option fields that DO NOT match the query 
         * bitmask are set !O_SELECTABLE 
         */
        for (i=0; i<nitems; i++) {
                if (item_opts(item[i]) == O_SELECTABLE) {
                        key_noun(*((uint32_t *)item_userptr(item[i])));
                        if ((focused->options & query) != query)
                                item_opts_off(item[i], O_SELECTABLE);
                }
        }

        sort_noun_menu(op, 0);  /* Selectable items get promoted to top */
        scr_refresh();
}



/*
 * pattern_noun_menu -- perform pattern matching on a given noun menu
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void *pattern_noun_menu(int op)
{
        #define PATTERN (menu_pattern(nounmenu->menu))
        #define COLOR_NO_MATCH PUR_RED 

        static bool firstcall;
        static int status;
        int c;

        if (firstcall == true) 
                goto PATTERN_START;
        else                   
                goto PATTERN_LISTEN;


        /* 
         * Highlight the pattern buffer and draw 
         * the prompt character
         */
        PATTERN_START:
                open_noun_menu(op);
                firstcall = false;

                goto PATTERN_LISTEN;

        /*
         * Feed input into the menu driver's 
         * pattern handler
         */
        PATTERN_LISTEN:
                while ((c=getch()), c!='\n') {
                        switch (c) {
                        case KEY_ESC:
                                if (*(menu_pattern(nounmenu[op]->menu))=='\0') 
                                        goto PATTERN_END;
                                menu_driver(nounmenu[op]->menu, REQ_CLEAR_PATTERN);
                                break;
                        case KEY_BACKSPACE:
                                status = menu_driver(nounmenu[op]->menu, REQ_BACK_PATTERN);
                                break;
                        default:
                                status = menu_driver(nounmenu[op]->menu, c);
                                break;
                        }
                        werase(nounmenu[op]->buf);
                        if (status == E_NO_MATCH)
                                wcolor_set(nounmenu[op]->buf, COLOR_NO_MATCH, NULL);
                        else
                                wcolor_set(nounmenu[op]->buf, PUR_WHITE, NULL);

                        if (op == SUBJECT)
                                wprintw(nounmenu[op]->buf, "\"%s\"", menu_pattern(nounmenu[op]->menu));
                        else
                                wprintw(nounmenu[op]->buf, "  \"%s\"", menu_pattern(nounmenu[op]->menu));

                        wrefresh(nounmenu[op]->buf);
                        return (current_item(nounmenu[op]->menu));
                }

        /*
         * Erase the menu's internal pattern 
         * buffer and the buffer window, then
         * reinitialize the 'firstcall' bool.
         */
        PATTERN_END:
                menu_driver(nounmenu[op]->menu, REQ_CLEAR_PATTERN);
                werase(nounmenu[op]->buf);
                wcolor_set(nounmenu[op]->buf, PUR_PURPLE, NULL);
                win_refresh(nounmenu[op]->buf);
                scr_refresh();
                firstcall = true;
                return (NULL);
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

