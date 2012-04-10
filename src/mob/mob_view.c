#include <stdlib.h>
#include "mob_model.h"

////////////////////////////////////////////////////////////////////////////////

void mob_youarehere(struct mob_t *mob)
{
        static wchar_t *wch=L"⬇";
}


void move_mob(struct mob_t *mob, int dir)
{
        if (dir != 'u' && dir != 'd' && dir != 'l' && dir != 'r') return;

        int y = ufo_y(mob, ufo);
        int x = ufo_x(mob, ufo);

        switch (dir) {
        case 'u':       
                ufo_up(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj))
                        ufo_down(mob, ufo);
                break;
        case 'd':       
                ufo_down(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj)) 
                        ufo_up(mob, ufo);
                break;
        case 'l':       
                ufo_left(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj))
                        ufo_right(mob, ufo);
                break;
        case 'r':      
                ufo_right(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj))
                        ufo_left(mob, ufo);
                break;
        }

        // Scroll down at bottom boundary
        if (ufo_y(mob, ufo) == LINES-10) {
                roll_map(GLOBE, 'd');
                ufo_up(mob, ufo);
        }
        // Scroll right at right boundary
        if (ufo_x(mob, ufo) == COLS-10) {
                roll_map(GLOBE, 'r');
                ufo_left(mob, ufo);
        }

        move_panel(mob->pan, ufo_y(mob, ufo), ufo_x(mob, ufo));
        update_panels();

        path_push(&mob->path, y, x);
        doupdate();
}
