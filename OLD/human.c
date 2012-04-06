#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../pan/test.h"
#include "../lib/redblack/rb.h"
#include "../lib/ufo.h"
#include "mob.h"


