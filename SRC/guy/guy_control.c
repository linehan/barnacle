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
enum operator_modes { EXITING, STARTING,
                      NOUNMENU, ATTRIBUTES, PROFESSION, VITALS, 
                      PATTERN, ACTION };

#define RESET -1 
bool mode_changed;
int  mode;


inline void setmode(int newmode)
{
        if (mode == newmode) return;
        if (newmode == RESET) mode_changed ^= 1;
        else {
                mode = newmode;
                mode_changed = 1;
        }
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                  operate                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void operate_on(void *noun)
{
        uint32_t key = (noun!=NULL) ? *(uint32_t *)noun : 0;
       
        if (mode_changed) {
                if (mode != EXITING) {
                }
                setmode(RESET);
        }

        install_key(key);

        switch(mode) {
        case EXITING:
                view_noun_grey();
                close_nouns();
                return;
        case NOUNMENU:
                view_nouns();
                break;
        case ATTRIBUTES:
                view_attributes();
                break;
        case ACTION:
                focus(key);
                focused->action = 1;
        case VITALS:
                view_vitals();
                break;

        }
        view_noun();
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                 select                                     //
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

        #define DEFAULT_MODE VITALS

        ITEM *item = NULL;
        int c;

        // First run
        setmode(DEFAULT_MODE);
        item=current_item(noun_menu);
        operate_on(item_userptr(item));

        while ((c = getch())) 
        {
                switch (c)
                {
                case '/': 
                        while ((item = (ITEM *)get_pattern()) != NULL);
                                operate_on(item_userptr(item));
                        break;
                case '!':
                        setmode(ACTION);
                        break;
                case 'k':
                        menu_driver(noun_menu, REQ_PREV_ITEM);
                        break;
                case 'j':
                        menu_driver(noun_menu, REQ_NEXT_ITEM);
                        break;
                case 'n':
                        menu_driver(noun_menu, REQ_PREV_MATCH);
                        break;
                case 'p':
                        menu_driver(noun_menu, REQ_NEXT_MATCH);
                        break;
                case 'P':
                        setmode(PROFESSION);
                        break;
                case 's':
                        setmode(ATTRIBUTES);
                        break;
                case 'v':
                        setmode(VITALS);
                        break;
                case 'O':
                        setmode(NOUNMENU);
                        break;
                case KEY_ESC:
                case 'o':
                        setmode(EXITING);
                        operate_on(item_userptr(item));
                        return;
                }
                item=current_item(noun_menu);
                operate_on(item_userptr(item));

                scr_refresh();
        }
}
