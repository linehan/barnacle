// vim:fdm=marker
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <stdint.h>
#include <math.h>

#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../geo/terrain.h"
#include "../geo/weather.h"
#include "../pan/instruments.h"
#include "../pan/test.h"

static const double pi = 3.1415926;

float T; // Period
float A; // Amplitude
float w; // Angular frequency in radians/sec
  int t; // Time ticker

static const wchar_t wavwch[]=L" ▁▂▃▄▅▆▇█";
cchar_t *wavcch;

/*
  Adjust values for the wave's amplitude (_A) and period (_T). Also 
  re-calculates the angular frequency w to reflect any adjustments.
*/
void adj_wave(float _A, float _T)
{
        T = _T;
        A = _A;
        w = ((2.0*pi)/T);
}
/*
  Initialize the cchar_t array at *wavcch and set the default wave
  parameters.
*/
void init_wave(void)
{
        int i;
        int len;

        adj_wave(4.0, 16.0);
        t = 0; 

        len    = (int)(wcslen(wavwch)+1);
        wavcch = malloc(len * sizeof(cchar_t));
        for (i=0; i<len; i++) {
                setcchar(&wavcch[i], &wavwch[i], 0, UND_SEA, NULL);
        }
}
/*
  Returns the height of a the scaled to [-4,4].
*/
int waveheight(int t)
{
        return ((int)((A * sin(w*(float)t)))+4);
}
/*
  Selects the appropriate cchar_t from the wavcch[] array, indexed by
  the value of waveheight. This character is drawn on the WINDOW at
  *win.
*/
void wavefill(cchar_t **bucket, int n)
{
        int i;
        for (i=0; i<n; i++) { 
               bucket[i] = &wavcch[waveheight(i)];
        }
}
void printwave(WINDOW *win)
{                          
        wadd_wch(win, &wavcch[waveheight(t++)]);
}
