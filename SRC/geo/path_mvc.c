#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../gfx/brnoise.h"
#include "../lib/morton.h"
#include "path_mvc.h"

/*
 * Create a new struct path_t
 */
struct path_t *new_path(int yofs, int xofs, int len)
{
        int i;
        struct path_t *new = malloc(sizeof(struct path_t));

        new->origin = mort(yofs, xofs);

        new->fifo = FIFO_INIT;
        fifo_res(&new->fifo, len);
        for (i=0; i<FIFO_BUFSIZE; i++) 
                fifo_enq(&new->fifo, new->origin);
        
        for (i=0; i<SEG_N; i++)
                new->seg[i] = (len/SEG_BASELEN)*SEG_DEFAULT[i];

        return (new);
}

/*
 * Initialize the relative (y,x) offset from which a breadcrumb path 
 * (think of the game "snake") will emanate. For the trivial case of 
 * a 1x1 panel, this is not necessary, but with larger panels, a 
 * specific tile offset may need to be designated.
 *
 * Additionally, indicate the maximum length of the path.
 */
void init_path(struct path_t *path, int yofs, int xofs, int len)
{
        int i;

        path->fifo = FIFO_INIT;
        fifo_res(&path->fifo, len);
        path->origin = mort(yofs, xofs);

        for (i=0; i<FIFO_BUFSIZE; i++) 
                fifo_enq(&path->fifo, path->origin);

        for (i=0; i<SEG_N; i++)
                path->seg[i] = SEG_DEFAULT[i];
}

/*
 * Add (push) a coordinate to the struct path_t FIFO
 */
void path_push(struct path_t *path, int y, int x)
{
        y += (int)trom_y(path->origin);
        x += (int)trom_x(path->origin);

        fifo_enq(&path->fifo, mort(y, x));
}

/*
 * Remove (pop) an entry from the tail end of the struct path_t FIFO
 */
void path_pop(struct path_t *path)
{
        fifo_enq(&path->fifo, fifo_deq(&path->fifo)); // oroborific!
}


void path_seg(struct path_t *path, int head, int trans1, int body, int trans2, int tail)
{
        path->seg[0] = head;
        path->seg[1] = trans1;
        path->seg[2] = body;
        path->seg[3] = trans2;
        path->seg[4] = tail;
}



void draw_path(WINDOW *win, struct path_t *path)
{
        uint32_t pos;
        cchar_t cch[5];
        bool spool;
        int y;
        int x;
        int i=0;
        int j=0;

        for (spool = fifo_dmp(&path->fifo, NULL);
             spool;
             spool = fifo_dmp(&path->fifo, &pos)) 
        {

                setcchar(&cch[4], &BRDOT[6][roll_fair(DOT6)], 0, SEA_MED, NULL);
                setcchar(&cch[3], &BRDOT[3][roll_fair(DOT3)], 0, SEA_MED, NULL);
                setcchar(&cch[2], &BRDOT[2][roll_fair(DOT2)], 0, SEA_MED, NULL);
                setcchar(&cch[1], &BRDOT[1][roll_fair(DOT1)], 0, SEA_MED, NULL);
                setcchar(&cch[0], &BRDOT[1][roll_fair(DOT1)], 0, SEA__MED, NULL);

                y=(int)trom_y(pos);
                x=(int)trom_x(pos);

                if (path->seg[j] < i++) j++;
                            
                switch (j)
                {
                case 0:
                        mvwadd_wch(win, y, x, &cch[j]);
                        break;
                case 1:
                        mvwadd_wch(win, y, x, &cch[j]);
                        break;
                case 2:
                        mvwadd_wch(win, y, x, &cch[j]);
                        break;
                case 3:
                        mvwadd_wch(win, y, x, &cch[j]);
                        break;
                case 4:
                        mvwadd_wch(win, y, x, &cch[j]);
                        break;
                }
                wrefresh(win);
        }
}
