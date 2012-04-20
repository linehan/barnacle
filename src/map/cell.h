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
};

struct cellpath_t {
        struct list_head path;
        int n;
};


bool same_cell(struct cell_t *a, struct cell_t *b);
struct cell_t *new_cell(uint32_t y, uint32_t x);
struct cell_t *cellpath_goal(struct cellpath_t *path);
struct cell_t *cellpath_start(struct cellpath_t *path);
struct cell_t *cell_parent(struct cell_t *cell);
struct cell_t *cell_after(struct cell_t *cell, struct cell_t *match);

#endif
