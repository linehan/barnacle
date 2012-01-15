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

#include "lib/list/list.h"
#include "lib/memmacro.h"
#include "sprite.h"
#include "lib/gfx.h"
#include "boat.h"
#include "dice.h"
#include "palette.h"
#include "test.h"
/******************************************************************************/
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
        int i;

        wind_IO = MALLOC(sem_t);
        sem_init(wind_IO, 0, 1);

        g_windstate.dir = 0;
        g_windstate.str = 2; 
        g_windstate.pre = 5;

}
/* Access the global wind state */
int get_wind(const char *string)
{
        int buf;
        sem_wait(wind_IO);
        if ((strcmp(string, "dir")) == 0) 
                buf = g_windstate.dir;
        else if ((strcmp(string, "str")) == 0) 
                buf = g_windstate.str;
        else if ((strcmp(string, "pre")) == 0) 
                buf = g_windstate.pre;
        else {
                perror ("INVALID ARGUMENT TO get_wind()!");
                buf = 0;
        }
        sem_post(wind_IO);
        return buf;
}
/* Set the global wind state */
void set_wind(const char *string, int a)
{
        sem_wait(wind_IO);

        if ((strcmp(string, "dir")) == 0)
                g_windstate.dir = a;
        else if ((strcmp(string, "str")) == 0) 
                g_windstate.str = a;
        else if ((strcmp(string, "pre")) == 0) 
                g_windstate.pre = a;
        else perror ("INVALID ARGUMENT TO get_wind()!");

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
