#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/ui/stdmenu.h"
#include "noun_model.h"
#include "noun_control.h"




void sort_noun_menu(int op, uint32_t sort);
void query_noun_menu(int op, uint32_t query, int sort);
void *pattern_noun_menu(int op);




/*
 * sort_noun_menu -- sort the noun menu items according to some criteria
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 *
 * Notes
 * Selectable items are swapped with the first non-selectable item in the
 * list until the bottom of the list is reached. This is not a very
 * adventerous sort, what can I say.
 *
 * It is important that the menu be unposted, then re-coupled with the
 * ITEM ** array, and then re-posted, in that order.
 */
void sort_noun_menu(int op, uint32_t sort)
{
        struct stdmenu_t *nounmenu; /* Big dog */
        ITEM **item; /* All the items in the menu */
        ITEM *tmp;   /* For swapping items during sort */
        int nitems;  /* Number of items in total */
        int i;
        int j;

        nounmenu = get_noun_struct(op);
        nitems   = nounmenu->nitems;
        item     = nounmenu->item;
        tmp      = NULL;

        for (i=0; i<nitems; i++) {
                if (item_opts(item[i]) == O_SELECTABLE) {
                        /* Swap match with first non-selectable item */
                        tmp = item[i];
                        for (j=0; j<nitems; j++) {
                                if (item_opts(item[j]) != O_SELECTABLE)
                                        break; 
                        } 
                        item[i] = item[j];
                        item[j] = tmp;
                }
        }

        /* Make the MENU display the sorted items */
        unpost_menu(nounmenu->menu);
        set_menu_items(nounmenu->menu, nounmenu->item);
        post_menu(nounmenu->menu);
        scr_refresh();
}


/*
 * query_noun_menu -- list nearby nouns in order of increasing distance
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 *
 * Notes
 * Queries are bit masks representing various options that can be set 
 * to discriminate between nouns in some way. The query action consists
 * of AND-ing the query mask with a noun's options, and observing the
 * result. 
 *
 * If the result is equal to the query, the noun has a bit set at the 
 * location of the query bit, and we can say that the noun matches 
 * the query. 
 *
 * Nouns which fail to match the query are designated "unselectable", 
 * and the menu driver renders them as "greyed out". Depending on the
 * option passed as 'opt', the menu items will be sorted so that the
 * items which match the query are at the top.
 */
void query_noun_menu(int op, uint32_t query, int sort)
{
        struct stdmenu_t *nounmenu; 
        ITEM **item; /* All the items in the menu */
        int nitems;  /* Number of items in total */
        int i;

        nounmenu = get_noun_struct(op);
        nitems   = nounmenu->nitems;
        item     = nounmenu->item;

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

        if (sort)
                sort_noun_menu(op, 0);  /* Selectable items get promoted */

        scr_refresh();
}



/*
 * pattern_noun_menu -- perform pattern matching on a given noun menu
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 *
 * Notes
 * This is one of the pseudo-coroutines employed by the noun controller.
 * Once the appropriate mode is triggered, the master FSM is locked out
 * temporarily, and this function will be interpreting all keyboard input.
 *
 * The reason for this is that, obviously, alphanumeric input meant to
 * be interpreted as the literal elements of a pattern string would be
 * switched by the master FSM as though they were commands. 
 *
 * Once the appropriate keycode is received, this function will emit a 
 * NULL pointer to the noun controller, causing it to yield to the 
 * master FSM, putting things back the way they were. 
 */
void *pattern_noun_menu(int op)
{
        #define PATBUFLEN 200
        #define PATTERN (menu_pattern(nounmenu->menu))

        static struct stdmenu_t *nounmenu; 
        static WINDOW *bufwin;          /* Where to print the pattern buffer */
        static MENU *menu;              /* The menu being operated on */
        static bool is_firstcall=true;  /* Init guard */
        static char patbuf[PATBUFLEN];  /* The pattern buffer */
        static int patlen;              /* Current length of the pattern */
        static short color;             /* Color of bufwin */
        int c;

        /* So much harm! */
        if (is_firstcall) goto PATTERN_START; else goto PATTERN_LISTEN;

        /* 
         * Highlight the pattern buffer and draw 
         * the prompt character
         */
        PATTERN_START:
                is_firstcall = false;               /* We're past that now */
                nounmenu     = get_noun_struct(op); /* Make way */
                menu         = nounmenu->menu;      /* Enhance readability */
                bufwin       = nounmenu->buf;       /* Enhance readability */
                color        = PUR_WHITE;           /* Default pattern color */
                patbuf[0]    = '\0';                /* Make patbuf a string */
                open_noun_menu(op);                 /* Open menu */

                goto PATTERN_LISTEN; /* Enter the main loop below */

        /*
         * Feed input into the menu driver's 
         * pattern handler
         */
        PATTERN_LISTEN:
                while ((c = getch()), c != '\n')  {

                        switch (c) {
                        /* 
                         * ESC ends pattern matching mode 
                         */ 
                        case KEY_ESC:
                                goto PATTERN_END;
                        /* 
                         * Tab highlights the next match 
                         */
                        case '\t':
                                menu_driver(menu, REQ_NEXT_MATCH);
                                break;
                        /* 
                         * Shift+Tab highlights the prev match
                         */
                        case KEY_BTAB:
                                menu_driver(menu, REQ_PREV_MATCH);
                                break;
                        /* 
                         * Backspace does what you'd expect
                         */
                        case KEY_BACKSPACE:
                                patlen = strlen(patbuf);
                                patbuf[patlen-1] = '\0';
                                break;
                        /* 
                         * Otherwise, alphabetic input is appended 
                         * to patbuf 
                         */
                        default:
                                if (isalpha(c)) {
                                        patlen = strlen(patbuf);
                                        patbuf[patlen] = c;
                                        patbuf[patlen+1] = '\0';
                                }
                                break;
                        }

                        /* 
                         * If the pattern fails to match, clear the MENU's
                         * pattern buffer, set the cursor to the first item,
                         * and change the pattern buffer's text color
                         */
                        if (set_menu_pattern(menu, patbuf) == E_NO_MATCH) {
                                menu_driver(menu, REQ_CLEAR_PATTERN);
                                menu_driver(menu, REQ_FIRST_ITEM);
                                color = PUR_RED;
                        }
                        else
                                color = PUR_WHITE;

                        /* Clear the buffer window and apply the coloring */
                        werase(bufwin);
                        wcolor_set(bufwin, color, NULL);

                        /* Print with a 1-char offset for OBJECT window */
                        if (op == SUBJECT) wprintw(bufwin, "\"%s\"", patbuf);
                        else               wprintw(bufwin, " \"%s\"", patbuf);
                        wrefresh(bufwin);

                        /* 
                         * THIS IS THE NORMAL EXIT POINT.
                         *
                         * Return the current match to the noun controller,
                         * so it can be operated on if necessary, before the
                         * controller re-enters this function.
                         */
                        return (current_item(menu));
                }
        /*
         * This is the BIG GOODBYE... 
         *
         * Erase the menu's internal pattern buffer 
         * and the buffer window, then reinitialize 
         * the 'firstcall' bool.
         */
        PATTERN_END:
                menu_driver(menu, REQ_CLEAR_PATTERN); /* Just in case */ 
                werase(bufwin);                       
                wcolor_set(bufwin, PUR_PURPLE, NULL); /* Restore default */
                win_refresh(bufwin);
                scr_refresh();
                is_firstcall = true; /* When next we meet */

                return (NULL); /* NULL tells the noun controller we're done */
}
