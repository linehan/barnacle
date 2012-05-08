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

#include "noun.h"
#include "nounmenu.h"
#include "nounmenu_control.h"
#include "nounmenu_discrim.h"
#include "../verb/verb.h"
#include "../gfx/ui/stdmenu.h"



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                  modes                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
enum operator_modes { 
        OPSUBJECT,      /* Change operand to SUBJECT */
        STARTING,       /* Initialize */
        EXITING,        /* Exit */
        LAST,           /* Restore mode to previous */
        VITALS,         /* Display vitals of operand */
        PATTERN,        /* Pattern matching in progress */
        ACTION,         /* Initiate verb action */
        CANCEL,         /* Cancel verb action in progress */
        POPMENU,        /* Open the operand's menu (if closed) */
        REFRESHMENU,    /* Refresh the noun menu with the query results */
        TOGMENU         /* Toggle the menu of the operand */
};


struct stdmenu_t *nounmenu;

#define RESET -1 
bool mode_changed;
int  mode;
int  oldmode;
static int op;

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

        install_id(key, SUBJECT);

        switch (mode) {
        case STARTING:
                setmode(DEFAULT_MODE);
                break;
        case EXITING:
                nounmenu_print_name(PUR_WHITE);
                nounmenu->close(nounmenu);
                return;
        case PATTERN:
                scr_refresh();
                return;
        case TOGMENU:
                nounmenu->tog(nounmenu);
                setmode(DEFAULT_MODE);
                return;
        case REFRESHMENU:
                nounmenu->close(nounmenu);
                make_nounmenu(ALL_NOUNS);
                nounmenu->open(nounmenu);
                setmode(LAST);
                break;
        case POPMENU:
                nounmenu->open(nounmenu);
                setmode(DEFAULT_MODE);
                break;
        case CANCEL:
                setmode(VITALS);
                break;
        }
        switch(mode) {
        case VITALS:
                /*nounmenu_print_vitals();*/
                break;
        }
        nounmenu_print_name(PUR_PURPLE);
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
        MENU *menu = get_nounmenu();
        static ITEM *item;

        switch (ch) {

        /* Enter noun menu mode --------------------------- */
        case MODE_STARTED:
                if (mode == EXITING) 
                        setmode(LAST);
                else {
                        setmode(STARTING);
                        nounmenu = get_noun_struct();
                }
                break;

        /* Menu navigation -------------------------------- */
        /*case 'k':*/
                /*menu_driver(menu, REQ_PREV_ITEM);*/
                /*break;*/
        /*case 'j':*/
                /*menu_driver(menu, REQ_NEXT_ITEM);*/
                /*break;*/
        /*case 'K':*/
                /*if ((menu_driver(menu, REQ_SCR_UPAGE) == E_REQUEST_DENIED))*/
                        /*menu_driver(menu, REQ_FIRST_ITEM);*/
                /*break;*/
        /*case 'J':*/
                /*if ((menu_driver(menu, REQ_SCR_DPAGE) == E_REQUEST_DENIED))*/
                        /*menu_driver(menu, REQ_LAST_ITEM);*/
                /*break;*/


        /* Select / open the subject or object menu ------- */
        /*case 'i':*/
                /*if (!nounmenu_isvis())*/
                        /*setmode(POPMENU);*/

                /*nounmenu_focus(true);*/
                /*break;*/

        /* Toggle the selected menu ----------------------- */
        /*case '\n':*/
        /*case ' ':*/
                /*nounmenu_close();*/

                /*if (nounmenu_isvis()) {*/
                        /*nounmenu_focus(true);*/
                        /*nounmenu_focus(false);*/
                        /*setmode(op^1);*/
                /*}*/

                /*break;*/

        /* Refresh the contents of the selected menu ------ */
        /*case 'u':*/
                /*setmode(REFRESHMENU);*/
                /*break;*/

        /*[> Highlight (locate) the currently selected noun - <]*/
        /*case 'l':*/
                /*nounmenu_print_highlight();*/
                /*break;*/

        /*[> Feed input to the pattern matcher -------------- <]*/
        /*case '/': */
                /*setmode(PATTERN);*/
                /*while (item=(ITEM *)pattern_noun_menu(), item!=NULL)*/
                        /*operate_on(item_userptr(item));*/
                /*setmode(LAST);*/
                /*break;*/

        /*[> Initiate verb actions -------------------------- <]*/
        /*case 'x':*/
                /*setmode(CANCEL);*/
                /*break;*/

        /*[> Will be filters in the future ------------------ <]*/
        /*case 'p':*/
                /*query_noun_menu(NOUN_MOBILE, NOUN_DOSORT);*/
                /*break;*/
        /*case 's':*/
                /*sort_noun_menu(0);*/
                /*break;*/
        /*case 'v':*/
                /*setmode(VITALS);*/
                /*break;*/

        /* Exit noun menu mode ---------------------------- */
        case KEY_ESC:
        case 'm':
                setmode(EXITING);
                operate_on(item_userptr(item));
                return MODE_RESTORE;
        }

        /* Feed the noun key of the selected item into the operator routine */
        item = current_item(menu);
        operate_on(item_userptr(item));

        return MODE_PERSIST;
}

