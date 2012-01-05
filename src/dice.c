#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "palette.h"
#include "weather.h"
#include "control.h"

int roll(int sides)
{
   srand(time(NULL)); /* seed rand() with system time */
   return (rand() % sides);
}





   
   
