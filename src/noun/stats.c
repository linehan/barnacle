#include "../com/arawak.h"
#include "noun.h"
#include "stats.h"
#include "../gfx/ui/dock.h"
#include "../lib/textutils.h"



void print_stats(STATS stat)
{
        WINDOW *win = dock_win(STAT_WIN);

        werase(win);

        wpumpw(win, L"⦗%02u⦘⦗%02u⦘⦗%02u⦘", HP(stat), SP(stat), AP(stat));
}
