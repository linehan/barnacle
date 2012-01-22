// vim:fdm=marker
/*******************************************************************************
 FILENAME:  weather.c
 Weather and environmental (not those kind) variables that determine, among 
 other things, the state and rate of a ship at sail.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "../lib/llist/list.h"
#include "../lib/memmacro.h"
#include "../gen/dice.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../mob/boat.h"
#include "../pan/test.h"
/******************************************************************************/
enum wind_options{
        __dir__ = 0, 
        __str__ = 1, 
        __pre__ = 2 
};
struct windpackage {
        int dir;
        int str;
        int pre;
};
static struct windpackage g_windstate; /* the global wind state */
static sem_t *wind_IO;
/******************************************************************************/
/* Initialize weather globals */
void weather_init(void)
{
        wind_IO = MALLOC(sem_t);
        sem_init(wind_IO, 0, 1);

        g_windstate.dir = 0;
        g_windstate.str = 2; 
        g_windstate.pre = 0;

}
/* Access the global wind state */
int get_wind(int a)
{
        int buf = 0;
        sem_wait(wind_IO);
                switch (a) {
                case __dir__:
                        buf = g_windstate.dir;
                        break;
                case __str__:
                        buf = g_windstate.str;
                        break;
                case __pre__:
                        buf = g_windstate.pre;
                        break;
                }
        sem_post(wind_IO);
        return buf;
}
/* Set the global wind state */
void set_wind(int a, int val)
{
        sem_wait(wind_IO);
                switch (a) {
                case __dir__:
                        g_windstate.dir = val;
                        break;
                case __str__:
                        g_windstate.str = val;
                        break;
                case __pre__:
                        g_windstate.pre = val;
                        break;
                }
        sem_post(wind_IO);
}

/* Figure out what the wind will do */
/*int wind_roll(void)*/
/*{*/
        /*int d = get_wind("dir");*/
        /*int p = get_wind("pre");*/
        /*int h = get_boat("now", "hdg");*/

        /*if (p == d)*/
                /*return 0;*/

        /*int x = ((15+(p-d))%16);*/
        /*[> Proceed CW toward the prevailing direction <]*/
        /*if (x > HALFDIR) {*/
                /*set_wind("dir", ((16+(d-1))%DIRS)); */
                /*tumble_compass(2);*/
        /*[> Proceed CCW toward the prevailing direction <]*/
        /*} else if (x < HALFDIR) {*/
                /*set_wind("dir", ((d+1)%DIRS)); */
                /*tumble_compass(1);*/
        /*}*/
        /*return 0;*/
/*}*/
