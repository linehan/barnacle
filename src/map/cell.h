#pragma once
#ifndef __CELL_H
#define __CELL_H

#include "../lib/llist/list.h"


struct cell_t { 
        uint32_t key;          /* Morton code (perfect hash) */
        uint32_t y;            /* Cartesian y-coordinate */
        uint32_t x;            /* Cartesian x-coordinate */
        float g;               /* g(n) movement cost (used by A*) */
        float h;               /* h(n) heuristic (used by A*) */
        float f;               /* f(n) total movement cost (used by A*) */
        struct list_node node; /* Cells are nodes in a circular linked list */
        struct cell_t *parent;
        void (*die)(void *self);
};


struct cellpath_t {
        struct list_head path;
        int n;
};


bool same_cell(struct cell_t *a, struct cell_t *b);
struct cell_t *new_cell(uint32_t y, uint32_t x);
struct cell_t *cellpath_goal(struct list_head *path);
struct cell_t *cellpath_start(struct list_head *path);
struct cell_t *cell_parent(struct cell_t *cell);
struct cell_t *cell_after(struct cell_t *cell, struct cell_t *match);
struct cell_t *cell_dup(struct cell_t *cell);
struct cell_t *cellpath_next(struct list_head *path);

/*
 * The caller may request a specific neighbor
 */
#define NBR_MASK 0x0000000F
#define NBR(opt) ((opt) & NBR_MASK) /* The value of the neighbor byte */
#define NBR_N  0 
#define NBR_E  1 
#define NBR_S  2 
#define NBR_W  3 
#define NBR_NE 4
#define NBR_SE 5 
#define NBR_NW 6 
#define NBR_SW 7 
/*
 * The caller may request these additional options
 */
#define WLK_FW 0x00000010  /* Only proceed forward; do not backtrack */
#define WLK_MO 0x00000020  /* Only consider the Moore neighborhood */
#define WLK_VN 0x00000040  /* Only consider the von Neumann neighborhood */
#define WLK_DRUNK    0x00000080  /* Select a random neighbor (drunken walk) */
/*
 * Return the value of the specific option 'req' in the option word 'opt'
 */
#define OPT(opt,req) (((opt) & (req)) == (req)) ? true : false
/*
 * The number of cells in the respective neighborhoods
 */
#define NUM_VON_NEUMANN 4
#define NUM_MOORE       8

void cell_walk(struct cell_t **cell, int h, int w, int opt);
struct cell_t **mk_neighbors(struct cell_t *cell, int h, int w, int opt);


#endif
