#include "../../com/arawak.h"
#include "../../ai/astar.h"
#include "../test.h"
#include "../../eng/bytes.h"
#include "../../map/map.h"




static inline void ncurses_start(void)
{
        initscr();
        cbreak();	       /* Do not wait for newline (line buffering) */
        noecho();              /* Do not echo input to screen */
        curs_set(0);           /* Do not display cursor */
}




static inline void peekheap(struct astar_t *astar, struct cell_t *cell)
{
        #define PEEKSIZE 5
        int i;
        int xofs = 33;
        int yofs = 0;

        mvwprintw(DIAGNOSTIC_WIN, yofs++, xofs, "%u items in OPEN, out of %u possible\n",
                 astar->OPEN->n-1, astar->OPEN->max);
        for (i=BH_ROOT; i<astar->OPEN->n && i<PEEKSIZE; i++) {
                uint32_t key = bh_getkey(astar->OPEN, i);
                mvwprintw(DIAGNOSTIC_WIN, yofs++, xofs, "(%u) has priority %g (y:%u x:%u)\n", 
                         i, bh_getpri(astar->OPEN, i), 
                         trom_y(key), trom_x(key));
        }
        mvwprintw(DIAGNOSTIC_WIN, yofs++, xofs, "%u items in CLOSED, out of %u possible\n",
                 astar->CLOSED->n-1, astar->CLOSED->max);
        for (i=BH_ROOT; i<astar->CLOSED->n && i<PEEKSIZE; i++) {
                uint32_t key = bh_getkey(astar->CLOSED, i);
                mvwprintw(DIAGNOSTIC_WIN, yofs++, xofs, "(%u) has priority %g (y:%u x:%u)\n", 
                         i, bh_getpri(astar->CLOSED, i),
                         trom_y(key), trom_x(key));
        }
        mvwprintw(DIAGNOSTIC_WIN, yofs++, xofs, "Current cell: y:%u x:%u g:%g h:%g f:%g\n", 
                 cell->y, cell->x, cell->g, cell->h, cell->f);
}




static inline void print_map(struct astar_t *astar)
{
        int i;
        int j;

        for (i=0; i<astar->map->itr.rows; i++) {
                for (j=0; j<astar->map->itr.cols; j++) {
                        if (i == astar->start->y && j == astar->start->x)
                                mvwprintw(DIAGNOSTIC_WIN, i, j, "S");
                        else if (i == astar->goal->y && j == astar->goal->x)
                                mvwprintw(DIAGNOSTIC_WIN, i, j, "G");
                        else if ((get_byte(mx_val(astar->map, i, j), ALT)) <= 3)
                                mvwprintw(DIAGNOSTIC_WIN, i, j, ".");
                        else if ((get_byte(mx_val(astar->map, i, j), ALT)) > 3)
                                mvwprintw(DIAGNOSTIC_WIN, i, j, "#");
                }
        }
        update_panels();
        doupdate();
}



static inline void print_path(struct cell_t *cell)
{
        mvwprintw(DIAGNOSTIC_WIN, cell->y, cell->x, "%%");
        if (cell->parent)
                print_path(cell->parent);
        else
                mvwprintw(DIAGNOSTIC_WIN, cell->y, cell->x, "S");
}




static inline void test_rig(struct astar_t *astar, struct cell_t *current)
{
        print_map(astar);
        print_path(current);
        //peekheap(astar, current);
        doupdate();
}


static inline void astar_test(void)        
{
        #define SIZE 30 
        struct matrix_t *map;
        struct astar_t *astar = malloc(sizeof(struct astar_t));
        struct cell_t *goal = new_cell(25, 25);
        int i;
        int j;

        map = new_matrix(SIZE, SIZE);

        for (i=0; i<SIZE; i++) {
                for (j=0; j<SIZE; j++)
                        mx_set(map, i, j, 0);
        }

        for (i=0; i<SIZE; i++) {
                mx_set(map, 5, i, 3);
                mx_set(map, 18, i, 3);
        }
        mx_set(map, 5, 3, 0);
        mx_set(map, 5, 4, 0);
        mx_set(map, 18, 22, 0);
        mx_set(map, 18, 23, 0);

        astar_init(astar, map, 0, 0);

        a_star(astar, goal);
}

