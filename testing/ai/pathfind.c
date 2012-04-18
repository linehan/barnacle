#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <ncurses.h>
#include "morton.h"
#include "matrix.h"
#include "bh.h"
#include "unittests.h"


#define SETSIZE 9000
#define MAXPRI 2 


struct cell_t { 
        struct cell_t *parent;
        uint32_t key;
        int y; 
        int x; 
        int g; 
        int h;
        int f;
};

struct astar_t { 
        struct matrix_t *map;
        int rows;
        int cols;
        struct cell_t *start; 
        struct cell_t *goal; 
        struct cell_t *neighbor[4];
        struct bh_t *OPEN;
        struct bh_t *CLOSED;
};


struct cell_t *new_cell(int y, int x)
{
        struct cell_t *new = malloc(sizeof(struct cell_t));
        new->y = y;
        new->x = x;
        new->key = (uint32_t)MORT(y, x);
        return (new);
}


/* Allocate and stuff */
struct astar_t *new_astar(struct matrix_t *map, int rows, int cols, int st_y, int st_x, int go_y, int go_x)
{
        struct astar_t *new = malloc(sizeof(*new));
        
        new->map    = map;
        new->rows   = rows;
        new->cols   = cols;
        new->start  = new_cell(st_y, st_x);
        new->goal   = new_cell(go_y, go_x);
        new->OPEN   = new_bh(new->rows * new->cols);
        new->CLOSED = new_bh(new->rows * new->cols);

        return (new);
}



/*
 * same_cell -- if the x and y members of both cells match, they are the same 
 * @a: pointer to a cell
 * @b: pointer to a cell
 * Returns: TRUE if they are the same cell, else returns FALSE 
 */
bool same_cell(struct cell_t *a, struct cell_t *b)
{
        return (a->x==b->x && a->y==b->y) ? true : false;
}


/*
 * g_cost -- estimate movement cost from one cell to another  
 * @a: pointer to a cell
 * @b: pointer to a cell
 * Returns: distance between a and b, as a float
 */
int mov_cost(struct cell_t *a, struct cell_t *b)
{
        return abs((a->x - b->x)) + abs((a->y - b->y));
}


/*
 * g_cost -- the path cost from the starting position to the current position 
 * @astar: pointer to the astar object (contains start cell)
 * @cell: pointer to the current cell
 */
int g_cost(struct astar_t *astar, struct cell_t *cell)
{
        return mov_cost(astar->start, cell);
}


/*
 * h_cost -- return a heuristic estimate of the distance to the goal 
 * @astar: pointer to the astar object (contains goal cell)
 * @cell: pointer to the current cell 
 */
int h_cost(struct astar_t *astar, struct cell_t *cell)
{
        int move_cost;
        int D;          /* The heuristic coefficient */

        if (cell->x < 0 || cell->x >= astar->cols
        ||  cell->y < 0 || cell->y >= astar->rows)
                return 0;
        else {
                move_cost = mov_cost(cell, astar->goal);
                return move_cost; 
        }
}




struct cell_t *get_cell(struct bh_t *bh, struct cell_t *cell)
{
        struct cell_t *ptr;
        int i;

        for (i=ROOT; i<bh->n; i++) {
                ptr = (struct cell_t *)bh_peek(bh, i);
                if (same_cell(ptr, cell))
                        return ptr;
        }
        return NULL;
}



void lookheap(struct bh_t *bh, const char *name)
{
        int i;
        printf("%d items in %s, out of %d possible\n", bh->n-1, name, bh->max);

        for (i=ROOT; i<bh->n; i++) {
                struct cell_t *tmp = (struct cell_t *)bh_peek(bh, i);
                if (tmp) {
                        printf("y:%d x:%d g:%d h:%d f:%d\n", 
                               tmp->y, tmp->x, tmp->g, tmp->h, tmp->f);
                } else {
                        printf("NULL\n");
                }
        }
}

void peekheap(struct astar_t *astar, struct cell_t *cell)
{
        #define PEEKSIZE 5
        int i;
        int yofs = astar->rows+1;

        mvprintw(yofs++, 0, "%d items in OPEN, out of %d possible\n",
                 astar->OPEN->n-1, astar->OPEN->max);
        for (i=ROOT; i<astar->OPEN->n && i<PEEKSIZE; i++) {
                uint32_t key = bh_getkey(astar->OPEN, i);
                mvprintw(yofs++, 0, "(%d) has priority %d (y:%u x:%u)\n", 
                         i, bh_getpri(astar->OPEN, i), 
                         trom_y(key), trom_x(key));
        }
        mvprintw(yofs++, 0, "%d items in CLOSED, out of %d possible\n",
                 astar->CLOSED->n-1, astar->CLOSED->max);
        for (i=ROOT; i<astar->CLOSED->n && i<PEEKSIZE; i++) {
                uint32_t key = bh_getkey(astar->CLOSED, i);
                mvprintw(yofs++, 0, "(%d) has priority %d (y:%u x:%u)\n", 
                         i, bh_getpri(astar->CLOSED, i),
                         trom_y(key), trom_x(key));
        }
        mvprintw(yofs++, 0, "Current cell: y:%d x:%d g:%d h:%d f:%d\n", 
                 cell->y, cell->x, cell->g, cell->h, cell->f);
}



void print_map(struct astar_t *astar)
{
        int i;
        int j;

        for (i=0; i<astar->rows; i++) {
                for (j=0; j<astar->cols; j++) {
                        if (i == astar->start->y && j == astar->start->x)
                                mvprintw(i, j, "S");
                        else if (i == astar->goal->y && j == astar->goal->x)
                                mvprintw(i, j, "G");
                        else if (mx_val(astar->map, i, j) >= 3)
                                mvprintw(i, j, ".");
                        else if (mx_val(astar->map, i, j) == 2)
                                mvprintw(i, j, "*");
                        else if (mx_val(astar->map, i, j) < 1)
                                mvprintw(i, j, "#");
                }
        }
}



void print_path(struct cell_t *cell)
{
        mvprintw(cell->y, cell->x, "%%");
        if (cell->parent)
                print_path(cell->parent);
        else
                mvprintw(cell->y, cell->x, "S");
}


uint32_t zcode(struct cell_t *cell)
{
        return MORT(cell->y, cell->x);
}

uint32_t mapval(struct astar_t *astar, int y, int x)
{
        return mx_val(astar->map, y, x);
}


void gen_neighbors(struct astar_t *astar, struct cell_t *cell)
{
        struct cell_t **neighbor;
        int tentative_g_score;
        bool tentative_is_better;
        int i;

        neighbor = malloc(4 * sizeof(struct cell_t *));

        if (((cell->x > 0)
        && (mapval(astar, cell->y, cell->x-1) > 0)))
        {
                neighbor[0] = new_cell(cell->y, cell->x-1);
        }
        if (((cell->y > 0)
        && (mapval(astar, cell->y-1, cell->x) > 0)))
        {
                neighbor[1] = new_cell(cell->y-1, cell->x);
        }
        if (((cell->x < astar->cols-1)
        && (mapval(astar, cell->y, cell->x+1) > 0)))
        {
                neighbor[2] = new_cell(cell->y, cell->x+1);
        }
        if (((cell->y < astar->rows-1)
        && (mapval(astar, cell->y+1, cell->x) > 0)))
        {
                neighbor[3] = new_cell(cell->y+1, cell->x);
        }
                
        for (i=0; i<4; i++) {
                /* Illegal neighbors won't have been allocated */
                if (neighbor[i] == NULL) 
                        continue;

                /* We don't need anything from CLOSED */
                if (bh_member_of(astar->CLOSED, neighbor[i]->key))
                        continue;

                tentative_g_score = cell->g + mov_cost(cell, neighbor[i]);

                /* Add to OPEN if it's not there already */
                if (!bh_member_of(astar->OPEN, neighbor[i]->key)) {
                        bh_add(astar->OPEN, 0, neighbor[i]->key, neighbor[i]);
                        neighbor[i]->h = h_cost(astar, neighbor[i]);
                        tentative_is_better = true;
                } else if (tentative_g_score < g_cost(astar, neighbor[i])) {
                        tentative_is_better = true; 
                } else {
                        tentative_is_better = false;
                }

                /* Give it the proper scores and change its priority */
                if (tentative_is_better) {
                        neighbor[i]->parent = cell;
                        neighbor[i]->g = tentative_g_score;
                        neighbor[i]->f = neighbor[i]->g + neighbor[i]->h;
                        bh_setpri(astar->OPEN, neighbor[i]->key, neighbor[i]->f);
                }
        }
}




bool astar(struct matrix_t *map, int rows, int cols, int starty, int startx, int goaly, int goalx)
{
        struct cell_t *current;
        struct astar_t *astar;
        int esc=0;
       
        astar  = new_astar(map, rows, cols, starty, startx, goaly, goalx);

        /* Set up the start tile and add to OPEN */
        astar->start->g = 0;        
        astar->start->h = h_cost(astar, astar->start);
        astar->start->f = astar->start->g + astar->start->h;

        bh_add(astar->OPEN, astar->start->key, astar->start->f, astar->start);
        

        while (!bh_empty(astar->OPEN)) {

                current = bh_pop(astar->OPEN); 

                if (same_cell(astar->goal, current)) {
                        print_path(current);
                        return true;
                }

                bh_add(astar->CLOSED, current->f, current->key, current);

                gen_neighbors(astar, current);

                
                /*if (esc++ > 200)*/
                        /*return false;*/

                clear();
                doupdate();
                print_map(astar);
                print_path(current);
                peekheap(astar, current);
                getch();

        }


        return false;
}


        












