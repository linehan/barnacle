// vim:fdm=marker
/* ==========================================================================
    FILENAME:  gfx.c
    This module makes extensive use of the "famous" doubly linked list 
    routines written by Rusty Russell <rusty@rustcorp.com.au> for use in 
    the Linux kernel. He later modified them to an even more general form, 
    which is implemented here. For more information, see the CCAN entry 
    at <http://ccodearchive.net/info/list.html>.
   ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>

#include "lib/list/list.h"
#include "palette.h"
/* ========================================================================== */
#define top_wnode(gfx) (WNODE *)list_top(gfx->wins, WNODE, node)

enum pan_options { 
        __pan__ = 0,
        __win__ = 1,
        __dim__ = 2,
        __sem__ = 3,
        __obj__ = 4,
        __h__   = 5, 
        __w__   = 6, 
        __y0__  = 7,
        __x0__  = 8, 
        __dy__  = 9, 
        __dx__  = 10, 
        __n__   = 11
};

enum ids { __bg__ = 0,
           __fg__ = 1,
           __an__ = 2,
           __hi__ = 3
};

/* A structure that bundles all the window dimensions */
typedef struct dimension_t {
        int h;  /* window height */
        int w;  /* window width  */
        int y0; /* window initial y */
        int x0; /* window initial x */
        int dy; /* window current y */
        int dx; /* window current x */
        int n;  /* number of windows */
} DIMS;

typedef struct environment_t {
        struct list_head wad;
        sem_t *sem;
} ENV;

/* A mobile data type containing some object */
typedef struct mob_t {
        PANEL *pan;
        DIMS   dim;
        ENV   *env;
        sem_t *sem;
        void  *obj;
} MOB;

/* A node in a linked list ("wad") of windows */
typedef struct win_wad {
        int id;
        int z;
        WINDOW *window;
        struct list_node node; 
} WNODE;

/* A node in a linked list ("wad") of graphics */
typedef struct gfx_wad {
        int id;
        PANEL *pan;
        DIMS   dim;
        struct list_head *wins;
        struct list_node node;
} GNODE;

sem_t *REFRESH_LOCK;
/* ========================================================================== */
/* Start the graphics engine */
void gfx_init(void)
{
        REFRESH_LOCK = (sem_t *)malloc(sizeof(sem_t));
        sem_init(REFRESH_LOCK, 0, 1);
}
/* Master refresh */
void master_refresh(void)
{
        sem_wait(REFRESH_LOCK);
                update_panels();
                doupdate();
        sem_post(REFRESH_LOCK);
}
ENV *new_env(void)
{
        ENV *new = malloc(sizeof(ENV));
        list_head_init(&(new->wad));
        new->sem = malloc(sizeof(sem_t));
        sem_init(new->sem, 0, 1);

        return new;
}
/* Add a new GNODE to a graphics wad */
void add_gfx(GNODE *node, struct list_head *head)
{
        list_add(head, &(node->node));
}
/* Initialize a new graphics node */
GNODE *new_gfx(int id, int h, int w, int y0, int x0, int n)
{
        WNODE *tmp;
        int i;

        GNODE *gfx = (GNODE *)malloc(sizeof(GNODE));

        gfx->wins = malloc(sizeof(struct list_head));
        list_head_init(gfx->wins);
        /* Populate the winwad */
        for (i=0; i<n; i++) {
                WNODE *new  = (WNODE *)malloc(sizeof(WNODE));
                new->window = newwin(h, w, y0, x0);
                list_add(gfx->wins, &(new->node)); /* add new to winwad */
        }
        /* Attach the first window of the winwad to the panel */
        tmp = list_top(gfx->wins, WNODE, node);
        gfx->pan = new_panel(tmp->window);

        /* Populate the DIMS with the supplied arguments */
        gfx->dim.h = h;
        gfx->dim.w = w;
        gfx->dim.y0 = y0;
        gfx->dim.x0 = x0;
        gfx->dim.dy = 0;
        gfx->dim.dx = 0;
        gfx->dim.n = n;

        gfx->id = id;

        return gfx;
}

WNODE *next_wnode(GNODE *gfx)
{
        WNODE *tmp;
        tmp = list_top(gfx->wins, WNODE, node);
              list_del_from(gfx->wins, &tmp->node);
              list_add_tail(gfx->wins, &tmp->node);
        tmp = list_top(gfx->wins, WNODE, node);
        return tmp;
}

/* Create and initialize a new MOB */
MOB *new_mob(void *ptr, ENV *env, int h, int w, int y0, int x0)
{
        MOB *mob = (MOB *)malloc(sizeof(MOB));

        mob->obj = ptr;

        mob->env = env;

        WINDOW *win = newwin(h, w, y0, x0);
        mob->pan = new_panel(win);

        mob->dim.h = h;
        mob->dim.w = w;
        mob->dim.y0 = y0;
        mob->dim.x0 = x0;
        mob->dim.dy = 0;
        mob->dim.dx = 0;
        mob->dim.n = 0;

        mob->sem = malloc(sizeof(sem_t));
        sem_init(mob->sem, 0, 1);

        return mob;
}
/* Simple collision hit test */
int hit_test(ENV *env, int y, int x)
{
        GNODE *tmp;
        int i = 0;

        sem_wait(env->sem);
                list_for_each(&(env->wad), tmp, node) {
                        if ((tmp->id != __bg__)) {
                                if (((x >= tmp->dim.x0) && 
                                    (x <= (tmp->dim.x0 + tmp->dim.w)) &&
                                    (y >= tmp->dim.y0) &&
                                    (y <= (tmp->dim.y0 + tmp->dim.h))))
                                        i++;
                        }
                }
        sem_post(env->sem);
        return i;
}
/* Move a MOB type one step in one of four directions, checking for window
   edges and collision detection. Threadsafe. */
void move_mob(MOB *mob, int dir)
{
        sem_wait(mob->sem);

                int y = mob->dim.dy;
                int x = mob->dim.dx;

                switch (dir) {
                case 'u':
                        if (((y>0)) && ((hit_test(mob->env, --y, x) == 0)))
                                mob->dim.dy--;
                        break;
                case 'd':
                        if ((y<LINES) && ((hit_test(mob->env, ++y, x) == 0)))
                                mob->dim.dy++;
                        break;
                case 'l':
                        if (((x>0)) && ((hit_test(mob->env, y, --x) == 0)))
                                mob->dim.dx--;
                        break;
                case 'r':
                        if (((x<COLS)) && ((hit_test(mob->env, y, ++x) == 0)))
                                mob->dim.dx++;
                        break;
                }
                move_panel(mob->pan, mob->dim.dy, mob->dim.dx);
        sem_post(mob->sem);
}
void step_all_env(ENV *env)
{
        WNODE *tmpwin;
        GNODE *tmpgfx;

        sem_wait(env->sem);
                list_for_each(&(env->wad), tmpgfx, node) {
                        if ((tmpgfx->id == __an__)) {
                                tmpwin = top_wnode(tmpgfx);
                                tmpwin = next_wnode(tmpgfx);
                                replace_panel(tmpgfx->pan, tmpwin->window);
                        }
                }
        sem_post(env->sem);
        master_refresh();
}
GNODE *get_id(struct list_head *head, int id)
{
        GNODE *tmp;
        list_for_each(head, tmp, node) {
                if ((tmp->id == id))
                        return tmp;
        }
        return NULL;
}

