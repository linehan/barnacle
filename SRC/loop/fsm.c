#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <ev.h>

#include "../gfx/gfx.h"
#include "fsm.h"

enum uimodes { 
        UI_COMPASS,
        UI_WORLDMAP,
        UI_NOUN,
        UI_VERB,
        UI_INSPECTOR,
        UI_PROMPT
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
        case UI_INSPECTOR:
                switch (input) 
                {
                case KEY_UP:
                        move_inspector('u');
                        break;
                case KEY_DOWN:
                        move_inspector('d');
                        break;
                case KEY_LEFT:
                        move_inspector('l');
                        break;
                case KEY_RIGHT:
                        move_inspector('r');
                        break;
                }
                break;
        case UI_NOUN:
                setmode(choose_noun(input));
                break;
        case UI_VERB:
                setmode(choose_verb(input));
                break;
        case UI_WORLDMAP:
                switch(input) 
                {
                case 'w':
                        roll_map(GLOBE, 'u');
                        break;
                case 'a':
                        roll_map(GLOBE, 'l');
                        break;
                case 's':
                        roll_map(GLOBE, 'd');
                        break;
                case 'd':
                        roll_map(GLOBE, 'r');
                        break;
                case 'j':
                        boat_control(get_boat("Afarensis"), input, 0);
                        break;
                case 'k':
                        boat_control(get_boat("Afarensis"), input, 0);
                        break;
                case 'h':
                        boat_control(get_boat("Afarensis"), input, 0);
                        break;
                case 'l':
                        boat_control(get_boat("Afarensis"), input, 0);
                        break;
                /*case ' ':*/
                        /*order_boat('h', 0);*/
                        /*break;*/
                case '@':
                        boat_control(get_boat("Afarensis"), 'f', 0);
                        break;
                /*case 'j':*/
                        /*order_boat('p', 0);*/
                        /*break;*/
                /*case 'k':*/
                        /*order_boat('s', 0);*/
                        /*break;*/
                case 'g':
                        boat_control(get_boat("Afarensis"), 'r', 0);
                        break;
                case 'f':
                        boat_control(get_boat("Afarensis"), 'R', 0);
                        break;
                }
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
                case 'c':
                        setmode(UI_NOUN);
                        setmode(MODE_STARTED);
                        break;
                case 'v':
                        setmode(UI_VERB);
                        setmode(MODE_STARTED);
                        break;
                case '`':
                        toggle_workbox();
                        break;
                case '~':
                        toggle_mm();
                        break;
                }
        }
}
