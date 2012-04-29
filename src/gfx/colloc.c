#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/llist/list.h"
#include "colloc.h"

#define NUM_COLORS COLORS
#define NUM_PAIRS  COLOR_PAIRS
#define __COLOR 0
#define __PAIR  1

struct list_head free_ring[2];
struct list_head used_ring[2];
int free_count[2];

struct colloc_node {
        int id;
        struct list_node node;
};


/* Helpers
``````````````````````````````````````````````````````````````````````````````*/

inline void init_rgb(int id, int r, int g, int b)
{
        init_color(id, r, g, b);
}

inline void push_free(int list, struct colloc_node *push)
{
        list_add(&free_ring[list], &push->node);
        free_count[list]++;
}

inline void push_used(int list, struct colloc_node *push)
{
        list_add(&used_ring[list], &push->node);
}

inline struct colloc_node *pop_free(int list)
{
        struct colloc_node *tmp;

        tmp = list_top(&free_ring[list], struct colloc_node, node);
        list_del_from(&free_ring[list], &tmp->node);

        free_count[list]--;

        return (tmp);
}

inline struct colloc_node *pop_used(int list, int id)
{
        struct colloc_node *tmp;
        struct colloc_node *nxt;

        list_for_each_safe(&used_ring[list], tmp, nxt, node) {
                if (tmp->id == id)
                        list_del_from(&used_ring[list], &tmp->node);
        }
        return (tmp);
}


void init_colloc(void)
{
        int i;

        list_head_init(&free_ring[__COLOR]); 
        list_head_init(&used_ring[__COLOR]); 
        list_head_init(&free_ring[__PAIR]); 
        list_head_init(&used_ring[__PAIR]); 

        for (i=0; i<NUM_COLORS; i++) {
                struct colloc_node *new = malloc(sizeof(*new));
                new->id = i;
                push_free(__COLOR, new);
        }
        for (i=0; i<NUM_PAIRS; i++) {
                struct colloc_node *new = malloc(sizeof(*new));
                new->id = i;
                push_free(__PAIR, new);
        }
}


int colloc(int r, int g, int b)
{
        struct colloc_node *color;

        if (list_empty(&free_ring[__COLOR]))
                return ERR_NOCOLORS;

        color = pop_free(__COLOR);
        init_rgb(color->id, r, g, b);
        push_used(__COLOR, color);

        return (color->id);
}
        
int palloc(int fg, int bg)
{
        struct colloc_node *pair;

        if (list_empty(&free_ring[__PAIR]))
                return ERR_NOPAIRS;

        pair = pop_free(__PAIR);
        push_used(__PAIR, pair);

        return (pair->id);
}

bool free_color(int id)
{
        struct colloc_node *color;

        color = pop_used(__COLOR, id);
        push_free(__COLOR, color);

        return true;
}


bool free_pair(int id)
{
        struct colloc_node *pair;

        pair = pop_used(__PAIR, id);
        push_free(__PAIR, pair);

        return true;
}
