#include <stdlib.h>
#include "../com/arawak.h"



#define SETSIZE 1000

struct bh_t *OPEN;
struct bh_t *CLOSED;

struct cell_t { int y; int x; };

/* Allocate and stuff */
inline void astar_init(void)
{
        int i;

        OPEN   = new_bh(SETSIZE);
        CLOSED = new_bh(SETSIZE);
}

struct cell_t *new_cell(int y, int x)
{
        struct cell_t *new = malloc(sizeof(struct cell_t));

        new->y = y;
        new->x = x;

        return (new);
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
 * distance_to_goal -- estimate distance from current cell to goal cell
 * @current: pointer to the current cell
 * @goal: pointer to the goal cell
 * Returns: distance between current and goal, as a float
 */
inline float distance_to_goal(struct cell_t *current, struct cell_t *goal)
{
        float xd;
        float yd;

        xd = (float)current->x - (float)goal->x;
        yd = (float)current->y - (float)goal->y;

        return (xd + yd);
}


/*
 * get_cost -- get the cost (priority) of a cell on the map
 * @x: the x-coordinate of the cell
 * @y: the y-coordinate of the cell
 * Returns: an integer value from 1 to MAXPRI 
 */
inline int get_cost(struct map_t *map, int x, int y)
{
        if (x < 0 || x >= map->ufo.box.w
        ||  y < 0 || y >= map->ufo.box.h
                return MAXPRI;
        else
                return (int)(get_nibble(map->mx->mx[y][x], ALT))
}




/*
 * gen_successors -- add the next successors to the priority queue
 * @bh: the binary heap priority queue
 * @current: the cell for which to generate the successors
 */
void gen_successors(struct bh_t *bh, struct cell_t *current)
{
        int parent_x; 
        int parent_y; 
        int pri;

        if (current) {
                parent_x = current->x;
                parent_y = current->y;
        }
        
        /* 
         * Add each of the four possible directions to the OPEN queue,
         * unless any of these directions cause the search to backtrack. 
         */

        if (pri=(get_cost(x-1, y), pri < MAXPRI) 
        && !(parent_x == x-1) 
        &&  (parent_y == y))
        {
                struct cell_t new = new_cell(x-1, y);
                bh_add(bh, new, pri);
        }       

        if (pri=(get_cost(x, y-1), pri < MAXPRI) 
        && !(parent_x == x) 
        &&  (parent_y == y-1))
        {
                struct cell_t new = new_cell(x, y-1);
                bh_add(bh, new, pri);
        }       

        if ((pri=get_cost(x+1, y), pri < MAXPRI)
        && !(parent_x == x+1) 
        &&  (parent_y == y))
        {
                struct cell_t new = new_cell(x+1, y);
                bh_add(bh, new, pri);
        }       

        if ((pri=get_cost(x, y+1), pri < MAXPRI) 
        && !(parent_x == x) 
        &&  (parent_y == y+1))
        {
                struct cell_t new = new_cell(x, y+1);
                bh_add(bh, new, pri);
        }       

        return true;
}



/*
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

reconstruct reverse path from goal to start
by following parent pointers
*/
void astar(int starty, int startx, int goaly, int goalx)
{
        if (!OPEN || !CLOSED || !square)
                astar_init();

        struct cell_t *start;
        struct cell_t *goal;
        struct cell_t *current;

        start = new_cell(starty, startx); 
        goal  = new_cell(goaly, goalx); 

        /* Here we go */
        gen_successors(OPEN, start);

        while (current = bh_pop(OPEN), !same_cell(current, goal)) {
         














