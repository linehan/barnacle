#include "../gfx/gfx.h"
#include "hexcolor.h"
#include "colortest.h"

bool init = false;

WINDOW *colortest_win;
PANEL *colortest_pan;

static wchar_t wch[]=L"█";
cchar_t cch[MAXRAMP];

#define PATCH 238


