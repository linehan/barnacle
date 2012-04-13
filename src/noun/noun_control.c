////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//       _---~~(~~-_.                                                         //
//     _{        )   )                                                        //
//   ,   ) -~~- ( ,-' )_                                                      //
//  (  `-,_..`., )-- '_,)                                                     //
// ( ` _)  (  -~( -_ `,  }                                                    //
// (_-  _  ~_-~~~~`,  ,' )                  noun_control.c                    //
//   `~ -^(    __;-,((()))                                                    //
//         ~~~~ {_ -_(())                                                     //
//                `\  }                                                       //
//                  { }                                                       //
//                   \`                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "../com/arawak.h"
#include <stdio.h>
#include <stdlib.h>

#include "../eng/fsm.h"
#include "../lib/stoc/stoc.h"
#include "../gfx/gfx.h"

#include "noun_view.h"
#include "noun_model.h"
#include "noun_control.h"
#include "../verb/verb_model.h"



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                  modes                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
enum operator_modes { 
        STARTING,       /* Initialize */
        EXITING,        /* Exit */
        LAST,           /* Restore mode to previous */
        ATTRIBUTES,     /* Display attributes of operand */
        PROFESSION,     /* Display profession of operand */
        VITALS,         /* Display vitals of operand */
        PATTERN,        /* Pattern matching in progress */
        ACTION,         /* Initiate verb action */
        CANCEL,         /* Cancel verb action in progress */
        OPSUBJECT,      /* Change operand to SUBJECT */
        OPOBJECT,       /* Change operand to OBJECT */
        POPMENU,        /* Open the operand's menu (if closed) */
        REFRESHMENU,    /* Refresh the noun menu with the query results */
        TOGMENU         /* Toggle the menu of the operand */
};


#define RESET -1 
bool mode_changed;
int  mode;
int  oldmode;
int  op;

void setmode(int newmode)
{
        #define DEFAULT_OPERAND SUBJECT

        if (newmode == mode) return;

        switch (newmode) {
        case RESET:
                mode_changed = false;
                return;
        case STARTING:
                op = DEFAULT_OPERAND;
                break;
        case LAST:
                mode = oldmode;
                return;
        case OPOBJECT:
                op = OBJECT;
                break;
        case OPSUBJECT:
                op = SUBJECT;
                break;
        default:
                if (newmode==STARTING) oldmode = newmode;
                else                   oldmode = mode;
                mode = newmode;
                mode_changed = true;
        }
}


inline int ismode(int somemode)
{
        return (mode == somemode) ? 1 : 0;
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                           operate on the noun                              //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void operate_on(void *noun)
{
        #define DEFAULT_MODE VITALS

        uint32_t key = (noun!=NULL) ? *(uint32_t *)noun : 0;
       
        if (mode_changed == true) 
                setmode(RESET);

        install_id(key, op);

        switch (mode) {
        case STARTING:
                install_id(key, op^1);
                setmode(DEFAULT_MODE);
                break;
        case EXITING:
                grey_current_noun(SUBJECT);
                grey_current_noun(OBJECT);
                close_noun_menu(SUBJECT);
                close_noun_menu(OBJECT);
                return;
        case PATTERN:
                scr_refresh();
                return;
        case TOGMENU:
                tog_noun_menu(op);
                setmode(DEFAULT_MODE);
                return;
        case REFRESHMENU:
                close_noun_menu(op);
                list_nouns(op, ALL_NOUNS);
                open_noun_menu(op);
                setmode(LAST);
                break;
        case POPMENU:
                open_noun_menu(op);
                setmode(LAST);
                break;
        case ACTION:
                if (op != SUBJECT) {
                        setmode(LAST);
                        break;
                }
                setmode(VITALS);
                break;
        case CANCEL:
                verb_cancel(key);
                setmode(VITALS);
                break;
        }
        switch(mode) {
        case ATTRIBUTES:
                view_attributes(op);
                break;
        case VITALS:
                view_vitals(op);
                view_vitals(op^1);
                break;
        }
        grey_current_noun(op^1);
        print_current_noun(op);
        scr_refresh();
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                      select subject or object noun                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
int choose_noun(int ch) 
{
        MENU *menu[2]={get_noun_menu(SUBJECT), get_noun_menu(OBJECT)};
        static ITEM *item;

        switch (ch) {

        /* Enter noun menu mode --------------------------- */
        case MODE_STARTED:
                if (mode == EXITING) 
                        setmode(LAST);
                else                 
                        setmode(STARTING);
                break;

        /* Menu navigation -------------------------------- */
        case 'k':
                menu_driver(menu[op], REQ_PREV_ITEM);
                break;
        case 'j':
                menu_driver(menu[op], REQ_NEXT_ITEM);
                break;
        case 'K':
                if ((menu_driver(menu[op], REQ_SCR_UPAGE) == E_REQUEST_DENIED))
                        menu_driver(menu[op], REQ_FIRST_ITEM);
                break;
        case 'J':
                if ((menu_driver(menu[op], REQ_SCR_DPAGE) == E_REQUEST_DENIED))
                        menu_driver(menu[op], REQ_LAST_ITEM);
                break;
        case 'n':
                menu_driver(menu[op], REQ_PREV_MATCH);
                break;
        case 'p':
                menu_driver(menu[op], REQ_NEXT_MATCH);
                break;

        /* Select / open the subject or object menu ------- */
        case 'o':
                if (op == OBJECT) setmode(TOGMENU);
                else              setmode(OPOBJECT);
                break;
        case 'i':
                if (op == SUBJECT) setmode(TOGMENU);
                else               setmode(OPSUBJECT);
                break;

        /* Toggle the selected menu ----------------------- */
        case '\n':
        case 'u':
                setmode(TOGMENU);
                break;

        /* Refresh the contents of the selected menu ------ */
        case 'r':
                setmode(REFRESHMENU);
                break;

        /* Feed input to the pattern matcher -------------- */
        case '/': 
                setmode(PATTERN);
                while (item=(ITEM *)pattern_noun_menu(op), item!=NULL)
                        operate_on(item_userptr(item));
                setmode(LAST);
                break;

        /* Initiate verb actions -------------------------- */
        case '!':
                setmode(ACTION);
                break;
        case 'x':
                setmode(CANCEL);
                break;

        /* Will be filters in the future ------------------ */
        case 'P':
                query_noun_menu(op, NOUN_MOBILE);
                break;
        case 'a':
                setmode(ATTRIBUTES);
                break;
        case 's':
                sort_noun_menu(op, 0);
                break;
        case 'v':
                setmode(VITALS);
                break;

        /* Exit noun menu mode ---------------------------- */
        case KEY_ESC:
        case 'm':
                setmode(EXITING);
                operate_on(item_userptr(item));
                return MODE_RESTORE;
        }

        /* Feed the noun key of the selected item into the operator routine */
        item = current_item(menu[op]);
        operate_on(item_userptr(item));

        return MODE_PERSIST;
}
