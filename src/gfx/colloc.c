#include "../com/arawak.h"
#include "../lib/llist/list.h"

#define NUM_COLORS COLORS
#define NUM_PAIRS  COLOR_PAIRS
#define NO_COLORS -1
#define NO_PAIRS  -2
#define _COLOR 0
#define _PAIR 1

struct list_head free[2];
struct list_head used[2];
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
        list_add(free[list], &push->node);
        free_count[list]++;
}

inline void push_used(int list, struct colloc_node *push)
{
        list_add(&used[list], &push->node);
}

inline struct color_node *pop_free(int list)
{
        struct color_node *tmp;

        tmp = list_top(&free[list], struct color_node, node);
        list_del_from(&free[list], &tmp->node);

        free_count[list]--;

        return (tmp);
}

inline struct color_node *pop_used(int list, int id)
{
        struct color_node *tmp;
        struct color_node *nxt;

        list_for_each_safe(&used[list], tmp, nxt, node) {
                if (tmp->id = id)
                        list_del_from(&used[list], &tmp->node);
        }
        return (tmp);
}


void init_colloc(void)
{
        int i;

        for (i=0; i<NUM_COLORS; i++) {
                struct color_node *new = malloc(sizeof(*new));
                new->id = i;
                push_free(_COLOR, new);
        }
        for (i=0; i<NUM_PAIRS; i++) {
                struct pair_node *new = malloc(sizeof(*new));
                new->id = i;
                push_free(_PAIR, new);
        }
}


int colloc(int r, int g, int b)
{
        struct colloc_node *color;

        if (list_empty(&free[_COLOR]))
                return NO_COLORS;

        color = pop_free(_COLOR);
        init_rgb(color->id, r, g, b);
        push_used(_COLOR, color);

        return (color->id);
}
        
int palloc(int fg, int bg)
{
        struct colloc_node *pair;

        if (list_empty(&free[_PAIR]))
                return NO_PAIRS;

        pair = pop_free(_PAIR);

        push_used(_PAIR, pair);

        return (pair->id);
}

bool free_color(int id)
{
        struct colloc_node *color;

        color = pop_used(_COLOR, id);
        push_free(color);

        return true;
}


bool free_pair(int id)
{
        struct colloc_node *pair;

        pair = pop_used(_PAIR, id);
        push_free(pair);

        return true;
}
