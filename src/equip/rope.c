#include <stdlib.h>
#include "../com/arawak.h"
#include "../map/cell.h"
#include "../lib/hash.h"
#include "rope.h"
#include "../map/terrain.h"
#include "../lib/stoc/stoc.h"


struct hashtable_t *rope_tbl;


uint32_t new_rope(int len, int test)
{
        struct rope_t *new;

        new = calloc(1, sizeof(struct rope_t));

        new->id   = mt_random();
        new->len  = len;
        new->test = test;
        new->head = NULL;
        new->end  = NULL;

        if (!rope_tbl)
                rope_tbl = new_hashtable(0);

        hashtable_add(rope_tbl, new->id, new);

        ropekey = new->id;

        return (new->id);
}


void rope_head(int y, int x, uint32_t key)
{
        struct rope_t *rope;
        rope = (struct rope_t *)hashtable_get(rope_tbl, key);
        rope->head = new_cell(y, x);
        rope->end  = rope->head;

        SET_TILE(ACTIVE, rope->head->y, rope->head->x, L_ROPE_ANCHOR);
}


void rope_drop(uint32_t key)
{
        struct rope_t *rope;
        int y;
        int x;
        int i;

        rope = (struct rope_t *)hashtable_get(rope_tbl, key);
        
        for (i=0; i<rope->len; i++) {
                y = (rope->end->y < LINES) ? rope->end->y+1 : rope->end->y;
                /*x = (rope->end->x < COLS)  ? rope->end->x+1 : rope->end->x;*/
                x = rope->end->x;
                struct cell_t *new = new_cell(y, x);
                new->parent = rope->end;
                rope->end = new;

                SET_TILE(ACTIVE, rope->end->y, rope->end->x, ROPE);
        }
}


