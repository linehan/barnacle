////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//       _---~~(~~-_.                                                         //
//     _{        )   )                                                        //
//   ,   ) -~~- ( ,-' )_                                                      //
//  (  `-,_..`., )-- '_,)                                                     //
// ( ` _)  (  -~( -_ `,  }                                                    //
// (_-  _  ~_-~~~~`,  ,' )                  guy_control.c                     //
//   `~ -^(    __;-,((()))                                                    //
//         ~~~~ {_ -_(())                                                     //
//                `\  }                                                       //
//                  { }                                                       //
//                   \`                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#define _XOPEN_SOURCE_EXTENDED = 1  
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <stdbool.h>
#include "../gen/dice.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "guy_view.h"
#include "guy_model.h"

#define KEY_ESC 27 // char value of ESC key

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                  modes                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
enum operator_modes { STARTING, EXITING, LAST, ATTRIBUTES, PROFESSION, VITALS, 
                      PATTERN, ACTION,
                      OPSUBJECT, OPOBJECT, POPMENU  };

#define RESET -1 
bool mode_changed;
int  mode;
int  oldmode;
int  op;

inline void setmode(int newmode)
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
                return;
        case OPSUBJECT:
                op = SUBJECT;
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
       
        if (mode_changed == true) setmode(RESET);

        install_key(key, op);

        switch(mode) {
        case STARTING:
                install_key(key, op^1);
                setmode(DEFAULT_MODE);
                view_vitals(SUBJECT);
                view_vitals(OBJECT);
        case EXITING:
                view_noun_grey(SUBJECT);
                view_noun_grey(OBJECT);
                close_nouns(SUBJECT);
                close_nouns(OBJECT);
                return;
        case POPMENU:
                open_nouns(op);
                setmode(LAST);
                break;
        }
        switch(mode) {
        case ATTRIBUTES:
                view_attributes();
                break;
        case ACTION:
                focus(key);
                focused->action = 1;
        case VITALS:
                view_vitals(op);
                break;
        }
        view_noun_grey(op^1);
        view_noun(op);
        scr_refresh();
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                      select subject or object noun                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void choose_noun(void) 
{
        /* ncurses sets the ESC key delay at 100ms by default, and this
         * is way too slow. According to Wolfram Alpha, 
         * 60hz == 0.0167s == 16.7ms, and is about equivalent to the time 
         * it takes a nerve impulse to travel the length of an avg. human 
         * body. For comparison, a typical human blink duration is 100ms-
         * 400ms, and that's just not what we're looking for here. */
        if (getenv ("ESCDELAY") == NULL) ESCDELAY = 25;

        MENU *menu[2]={get_noun_menu(SUBJECT), get_noun_menu(OBJECT)};
        ITEM *item = NULL;
        int c;  

        /* If called previously, the mode will be set to EXITING,
         * where the last call left it. */
        if (mode==EXITING) setmode(LAST);
        else               setmode(STARTING);

        item=current_item(menu[op]);
        operate_on(item_userptr(item));

        while ((c = getch())) 
        {
                switch (c)
                {
                case '/': 
                        while (item = (ITEM *)get_pattern(), item!=NULL)
                                operate_on(item_userptr(item));
                        break;
                case '!':
                        setmode(ACTION);
                        break;
                case 'k':
                        menu_driver(menu[op], REQ_PREV_ITEM);
                        break;
                case 'j':
                        menu_driver(menu[op], REQ_NEXT_ITEM);
                        break;
                case 'n':
                        menu_driver(menu[op], REQ_PREV_MATCH);
                        break;
                case 'p':
                        menu_driver(menu[op], REQ_NEXT_MATCH);
                        break;
                case 'P':
                        setmode(PROFESSION);
                        break;
                case 'a':
                        setmode(ATTRIBUTES);
                        break;
                case 'v':
                        setmode(VITALS);
                        break;
                case 'o':
                        if (op == OBJECT) setmode(POPMENU);
                        else              setmode(OPOBJECT);
                        break;
                case 's':
                        if (op == SUBJECT) setmode(POPMENU);
                        else               setmode(OPSUBJECT);
                        break;
                case KEY_ESC:
                case ' ':
                        setmode(EXITING);
                        operate_on(item_userptr(item));
                        return;
                }
                item=current_item(menu[op]);
                operate_on(item_userptr(item));

                scr_refresh();
        }
}
