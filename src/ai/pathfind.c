#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/bheap.h"
#include "../map/map.h"
#include "../lib/matrix.h"
#include "../lib/llist/list.h"


#define SETSIZE 9000

struct bh_t *OPEN;
struct bh_t *CLOSED;

struct cell_t { 
        int y; 
        int x; 
        int g; 
        int h;
        int f;
};

struct return_path {
        int y;
        int x;
        struct list_node node;
};


struct astar_t { 
        struct map_t *map;
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
        return (new);
}


/* Allocate and stuff */
struct astar_t *new_astar(struct map_t *map, int st_y, int st_x, int go_y, int go_x)
{
        struct astar_t *new = malloc(sizeof(*new));
        
        new->map    = map;
        new->start  = new_cell(st_y, st_x);
        new->goal   = new_cell(go_y, go_x);
        new->OPEN   = new_bh(SETSIZE);
        new->CLOSED = new_bh(SETSIZE);

        return (new);
}


void init_astar(struct astar_t *astar, struct map_t *map, int st_y, int st_x, int go_y, int go_x)
{
        astar->map    = map;
        astar->start  = new_cell(st_y, st_x);
        astar->goal   = new_cell(go_y, go_x);
        astar->OPEN   = new_bh(SETSIZE);
        astar->CLOSED = new_bh(SETSIZE);
}


/*
 * same_cell -- if the x and y members of both cells match, they are the same 
 * @a: pointer to a cell
 * @b: pointer to a cell
 * Returns: TRUE if they are the same cell, else returns FALSE 
 */
inline bool same_cell(struct cell_t *a, struct cell_t *b)
{
        return (a->x==b->x && a->y==b->y) ? true : false;
}


/*
 * g_cost -- estimate movement cost from one cell to another  
 * @a: pointer to a cell
 * @b: pointer to a cell
 * Returns: distance between a and b, as a float
 */
inline int mov_cost(struct cell_t *a, struct cell_t *b)
{
        int xd;
        int yd;

        xd = abs(a->x - b->x);
        yd = abs(a->y - b->y);

        return (xd + yd);
}


/*
 * g_cost -- return the movement cost between a cell and the goal
 * @astar: pointer to an astar type
 * @cell: pointer to a cell
 */
inline int g_cost(struct astar_t *astar, struct cell_t *cell)
{
        return (mov_cost(astar->goal, cell));
}


/*
 * h_cost -- return the heuristic cost of a cell on the map
 * @x: the x-coordinate of the cell
 * @y: the y-coordinate of the cell
 * Returns: an integer value from 1 to MAXPRI 
 */
inline int h_cost(struct astar_t *astar, struct cell_t *cell)
{
        int y = cell->y;
        int x = cell->x;

        if (x < 0 || x >= astar->map->ufo.box.w
        ||  y < 0 || y >= astar->map->ufo.box.h)
                return MAXPRI;
        else
                /*return (int)(get_nibble(astar->map->mx->mx[y][x], ALT));*/
                return 1;
}






void fill_costs(struct astar_t *astar, struct cell_t *cell)
{
        cell->g = mov_cost(cell, astar->start);
        cell->h = h_cost(astar, cell);
        cell->f = cell->g + cell->h;
}


bool member_of(struct bh_t *bh, struct cell_t *cell)
{
        struct cell_t *tmp;
        int i;

        for (i=0; i<bh->n; i++) {
                tmp = (struct cell_t *)bh_peek(bh, i);
                if (tmp && same_cell(tmp, cell))
                        return true;
        }
        return false;
}




struct cell_t **gen_neighbors(struct cell_t *cell)
{
        struct cell_t **neighbor = malloc(4 * sizeof(struct cell_t *));

        neighbor[0] = new_cell(cell->y, cell->x-1);
        neighbor[1] = new_cell(cell->y-1, cell->x);
        neighbor[2] = new_cell(cell->y, cell->x+1);
        neighbor[3] = new_cell(cell->y+1, cell->x);

        return (neighbor);
}



inline void spit(struct cell_t *cell)
{
        wprintw(DIAGNOSTIC_WIN, "cell: y:%d x:%d\n", cell->y, cell->x);
}

/*
 *
OPEN = priority queue containing START
CLOSED = empty set
while lowest rank in OPEN is not the GOAL:
  current = remove lowest rank item from OPEN
  add current to CLOSED
  for neighbors of current:
    cost = g(current) + movementcost(current, neighbor)
    if neighbor in OPEN and cost less than g(neighbor):
      remove neighbor from OPEN, because new path is better
    if neighbor in CLOSED and cost less than g(neighbor): **
      remove neighbor from CLOSED
    if neighbor not in OPEN and neighbor not in CLOSED:
      set g(neighbor) to cost
      add neighbor to OPEN
      set priority queue rank to g(neighbor) + h(neighbor)
      set neighbor's parent to current
*/

bool astar(struct map_t *map, int starty, int startx, int goaly, int goalx)
{
        struct cell_t *current;
        struct astar_t *astar;
        struct cell_t *neighbor[4];
        int tentative_g_score;
        bool tentative_is_better;
        int i;
        int breakout=0;

        astar = new_astar(map, starty, startx, goaly, goalx);

        astar->start->g = 0;
        astar->start->h = h_cost(astar, astar->start);
        astar->start->f = astar->start->g + astar->start->h;

        bh_add(astar->OPEN, astar->start->f, astar->start);

        /*while (!bh_is_empty(astar->OPEN)) {*/
        while (current = bh_pop(astar->OPEN), current) {

                /*current = bh_pop(astar->OPEN);*/

                /* If we're at the goal, we're done */
                if (same_cell(current, astar->goal))
                        return true;
                else
                        bh_add(astar->CLOSED, current->f, current);

                neighbor[0] = new_cell(current->y, current->x-1);
                neighbor[1] = new_cell(current->y-1, current->x);
                neighbor[2] = new_cell(current->y, current->x+1);
                neighbor[3] = new_cell(current->y+1, current->x);

                for (i=0; i<4; i++) {

                        /* If neighbor[i] in closed set, continue */
                        if (member_of(astar->CLOSED, neighbor[i])) 
                                continue;

                        tentative_g_score = current->g+mov_cost(current, neighbor[i]);

                        /* If neighbor[i] not in open set */ 
                        if (!member_of(astar->OPEN, neighbor[i])) {

                                bh_add(astar->OPEN, neighbor[i]->f, neighbor[i]);
                                neighbor[i]->h = h_cost(astar, neighbor[i]);
                                tentative_is_better = true;

                        } else if (tentative_g_score < neighbor[i]->g) {
                                tentative_is_better = true;
                        } else {
                                tentative_is_better = false;
                        }

                        if (tentative_is_better) {
                                neighbor[i]->g = tentative_g_score;
                                neighbor[i]->f = neighbor[i]->g + neighbor[i]->h;
                        }
                }

                wprintw(DIAGNOSTIC_WIN, "Items in OPEN:   %d\n"
                                        "Items in CLOSED: %d\n", 
                                        astar->OPEN->n,
                                        astar->CLOSED->n);
                if (breakout++ > 20)
                        break;
        }
        return false;
}


        












