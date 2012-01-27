#include <pthread.h>
#include <semaphore.h>
/* ========================================================================== */
/* Refresh the environment's pad by calling pnoutrefresh() followed by
 * doupdate(). */
#define ENV_REFRESH(env) prefresh(env->pad,                   \
                                  env->pminrow, env->pmincol, \
                                  env->sminrow, env->smincol, \
                                  env->smaxrow, env->smaxcol)
/* Synchronize the environment's pad by refreshing its VIRTUAL screen,
 * without yet refreshing the ACTUAL screen that the monitor displays.
 * There are many situations in which it is advantageous to modify the
 * virtual screen multiple times without calling for doupdate(). Using
 * both calls increases CPU usage and screen flicker. */
#define ENV_SYNC(env) pnoutrefresh(env->pad,                   \
                                   env->pminrow, env->pmincol, \
                                   env->sminrow, env->smincol, \
                                   env->smaxrow, env->smaxcol)
/* Calls ENV_SYNC, followed by doupdate(), to modify the virtual screen
 * one last time, and then write the virtual screen to the monitor. */
#define ENV_SYNCUP(env) ENV_SYNC(env) \
                        doupdate()

#define TOP_WNODE(gfx) (WNODE *)list_top(gfx->wins, WNODE, node)
#define LST_WNODE(gfx) (WNODE *)list_tail(gfx->wind, WNODE, node)
#define NXT_WNODE(gfx) (WNODE *)next_wnode(gfx)

#define TOP_GNODE(env) (GNODE *)list_top(env->wad, GNODE, node)
#define LST_GNODE(env) (GNODE *)list_tail(env->wad, GNODE, node)
#define NXT_GNODE(env) (GNODE *)next_gnode(env)

/* Panel options that can be used to identify the parameter that
 * one wishes returned from a GNODE structure */
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

/* Panel identifiers that can be used to identify a particular
 * type of GNODE when searching the graphics stack. */
enum ids { __bg__ = 0,
           __fg__ = 1,
           __an__ = 2,
           __hi__ = 3,
           __rw__ = 4,
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

/* A container to hold one entire "environment" */
typedef struct environment_t {
        struct list_head wad;
        WINDOW *bgw;
        PANEL  *bgp;
        sem_t  *sem;
} ENV;

typedef struct win_wad {
        int id;
        WINDOW *window;
        struct list_node node; 
} WNODE;

typedef struct graphics_node {
        int id;
        DIMS   dim;
        PANEL *pan;
        WNODE *W;
        struct list_head *wins;
        struct list_node node;
        int (*nextw)(const void *self);
        int (*prevw)(const void *self);
        int (*step)(const void *self);
} GNODE;

typedef struct map_plate {
        sem_t *sem;
        int id;
        int h;
        int w;
        GNODE *G;
        GNODE *BG;
        struct list_head *gfx; /* Terrain graphics */
        struct list_node node;
        int (*nextg)(const void *self);
        int (*prevg)(const void *self);
        int (*stepf)(const void *self);
} PLATE;

/* A mobile data type containing some object */
typedef struct mob_t {
        PANEL *pan;
        DIMS   dim;
        ENV   *env;
        sem_t *sem;
        void  *obj;
} MOB;
/* ========================================================================== */
void gfx_init(void);
void scr_refresh(void);
void vrt_refresh(void);

PLATE *new_plate(int type);

GNODE *new_gnode(int id, int h, int w, int y0, int x0, int n);
void   add_gfx(GNODE *node, struct list_head *wad);

MOB *new_mob(void *ptr, PLATE *pl, int h, int w, int y0, int x0);
void move_mob(MOB *mob, int dir);

int hit_test(PLATE *pl, int y, int x);
cchar_t *bg_tile(int type);




