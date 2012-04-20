#include <stdlib.h>
#include "../com/arawak.h"
#include "cell.h"
#include "../lib/morton.h"
#include "../lib/llist/list.h"

/*
 * new_cell -- allocate and initialize a new struct cell_t
 * @y: y-coordinate of cell
 * @x: x-coordinate of cell
 *
 * Notes
 * The 'key' member is used to perform lookups and identification of cells
 * in the OPEN or CLOSED sets. Each cell is identified by a kind of perfect 
 * hash called a Morton code, or Z-order code, which can be quickly computed 
 * by interleaving a pair of y and x coordinates. See "morton.h"
 */
struct cell_t *new_cell(uint32_t y, uint32_t x)
{
        struct cell_t *new = malloc(sizeof(struct cell_t));

        new->y   = y;
        new->x   = x;
        new->key = mort(y,x); 

        return (new);
}


/*
 * same_cell -- if the keys of both cells match, they are the same 
 * @a: pointer to a cell
 * @b: pointer to a cell
 */
bool same_cell(struct cell_t *a, struct cell_t *b)
{
        return (a->key == b->key) ? true : false;
}



struct cell_t *cell_parent(struct cell_t *cell)
{
        if (cell->parent)
                cell_parent(cell->parent);

        return cell;
}



/**
 * cellpath_start -- if the cell is a node in a path list, return the start node
 * @path: pointer to a path
 */
struct cell_t *cellpath_start(struct cellpath_t *path)
{
        if (list_empty(&path->path))
                return NULL;
        else
                return list_top(&path->path, struct cell_t, node);
}


/**
 * cellpath_goal -- if a path list is not empty, return the goal node
 * @path: pointer to a path
 */
struct cell_t *cellpath_goal(struct cellpath_t *path)
{
        if (list_empty(&path->path))
                return NULL;
        else
                return list_tail(&path->path, struct cell_t, node);
}


