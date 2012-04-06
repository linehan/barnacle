#pragma once
#ifndef __TESTPLEX_H
#define __TESTPLEX_H

#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include <stdlib.h>     
#include <stdio.h> 
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <locale.h>
#include <wchar.h>

#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#include "../lib/ncolor/src/ncolor.h"

#define NUM_TEST_WINDOWS 3
#define FIRST_TESTPAL_PAIRNUM 238
#define TESTPAL_PATH "/lib/ncolor/test/samples"
#define TESTPAL_NAME "meyers.ggr"

WINDOW  *TESTWIN[NUM_TEST_WINDOWS];
PANEL   *TESTPAN[NUM_TEST_WINDOWS];
PALETTE *TESTPAL;

void init_testoplex(void);
void testoplex(void);

#endif
