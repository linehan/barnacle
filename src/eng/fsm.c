#include "../com/arawak.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <ev.h>

#include "../noun/noun.h"
#include "../gfx/gfx.h"
#include "../mob/mob.h"
#include "../map/map.h"
#include "../test/test.h"
#include "fsm.h"

enum uimodes { 
        UI_COMPASS,
        UI_WORLDMAP,
        UI_NOUN,
        UI_VERB,
        UI_INSPECTOR,
        UI_PROMPT,
        UI_MOB,
        UI_BOAT
};

////////////////////////////////////////////////////////////////////////////////
static bool initialized;
static bool mode_locked;
static int mode;
static int oldmode;


static void setmode(int newmode)
{
        #define DEFAULT UI_WORLDMAP

        if (mode == newmode) return;
        if (mode == NOTOUCH) return;

        switch (newmode)
        {
        case MODE_PERSIST:
                mode_locked = true;
                break;
        case MODE_RELEASE:
                mode_locked = false;
                break;
        case MODE_RESTORE:
                mode = oldmode;
                mode_locked = true;
                break;
        case MODE_STARTED:
                operate_on(MODE_STARTED);
                break;
        case MODE_INITIAL:
                oldmode = mode = DEFAULT;
                mode_locked = false;
                initialized = true;
                break;
        case MODE_DEFAULT:
                newmode = DEFAULT;
        default:
                oldmode = mode;
                mode = newmode;
                mode_locked = true;
                break;
        }
}
        


////////////////////////////////////////////////////////////////////////////////
void operate_on(int input)
{
        setmode(MODE_RELEASE);

        switch (mode) 
        {
        case UI_NOUN:
                setmode(choose_noun(input));
                break;
        case UI_VERB:
                /*setmode(choose_verb(input));*/
                break;
        case UI_WORLDMAP:
                /*switch(input) */
                /*{*/
                /*case 'w':*/
                        /*map_roll(GLOBE, 'u');*/
                        /*break;*/
                /*case 'a':*/
                        /*map_roll(GLOBE, 'l');*/
                        /*break;*/
                /*case 's':*/
                        /*map_roll(GLOBE, 'd');*/
                        /*break;*/
                /*case 'd':*/
                        /*map_roll(GLOBE, 'r');*/
                        /*break;*/
                /*}*/
                break;
        case UI_INSPECTOR:
                inspect_control(input);
                break;
        case UI_BOAT:
                /*noun_set_state(get_noun("Afarensis"), VERB_Keyboard, input);*/
                break;
        case UI_MOB:
                noun_set_state(get_noun("Guy"), VERB_Keyboard, input);
                break;
        }
}

////////////////////////////////////////////////////////////////////////////////
void director(int input)
{
        if (initialized == false) setmode(MODE_INITIAL);

        operate_on(input);

        if (mode_locked == true) return;
        else {
                switch (input) 
                { 
                /*case '#':*/
                        /*TOGPAN(colortest_panel());*/
                        /*paint_ramp(0);*/
                        /*break;*/
                /*case '%':*/
                        /*testplex(0);*/
                        /*break;*/
                /*case ')':*/
                        /*inc_surface_flow();*/
                        /*break;*/
                /*case '(':*/
                        /*dec_surface_flow();*/
                        /*break;*/
                /*case '_':*/
                        /*rot_surface_rule();*/
                        /*break;*/
                case '1':
                        tab_sig(1);
                        break;
                case '2':
                        tab_sig(2);
                        break;
                case ',':
                        tab_cur('l');
                        break;
                case '.':
                        tab_cur('r');
                        break;
                case 'z':
                        dock_toggle();
                        break;
                case 'F':
                        fall_screen();
                        break;
                case '^':
                        map_swap();
                        break;
                case 'm':
                        setmode(UI_MOB);
                        setmode(MODE_STARTED);
                        break;
                case 'b':
                        setmode(UI_BOAT);
                        setmode(MODE_STARTED);
                        break;
                case 'i':
                case 'o':
                        setmode(UI_NOUN);
                        setmode(MODE_STARTED);
                        break;
                case 'v':
                        setmode(UI_VERB);
                        setmode(MODE_STARTED);
                        break;
                case '?':
                        setmode(UI_INSPECTOR);
                        setmode(MODE_STARTED);
                        break;
                case '`':
                        TOGPAN(CONSOLE_PAN);
                        doupdate();
                        test_request();
                        break;
                case '~':
                        TOGPAN(DIAGNOSTIC_PAN);
                        doupdate();
                        break;
                case '@':
                        loop_test();
                        break;
                case '&':
                        dock_say(L"䥚", "I am the computer king.");
                        break;
                case 't':
                        switch (getchar()) {
                        case 'd':
                                print_matrix(DIAGNOSTIC_WIN, ACTIVE->door);
                                break;
                        }
                        break;
                }
        }
}

