#include <pthread.h>
#include <semaphore.h>
/* ========================================================================== */
#define TOP_WNODE(gfx) (WNODE *)list_top(gfx->wins, WNODE, node)
#define LAST_WNODE(gfx) (WNODE *)list_tail(gfx->wind, WNODE, node)
#define NEXT_WNODE(gfx) (WNODE *)next_wnode(gfx)

#define TOP_GNODE(env) (GNODE *)list_top(env->wad, GNODE, node)
#define LAST_GNODE(env) (GNODE *)list_tail(env->wad, GNODE, node)
#define NEXT_GNODE(env) (GNODE *)next_gnode(env)

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

/* A container to hold one entire "environment" */
typedef struct environment_t {
        struct list_head wad;
        sem_t *sem;
} ENV;

/* A node in the windowset */
typedef struct win_wad {
        int id;
        int z;
        WINDOW *window;
        struct list_node node; 
} WNODE;

/* A node in the graphics wad */
typedef struct gfx_wad {
        int id;
        PANEL *pan;
        DIMS   dim;
        struct list_head *wins;
        struct list_node node;
} GNODE;

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
void master_refresh(void);

ENV *new_env(void);
void step_all_env(ENV *env);

GNODE *new_gfx(int id, int h, int w, int y0, int x0, int n);
void   add_gfx(GNODE *node, struct list_head *wad);
GNODE *find_gnode(struct list_head *head, int id);

WNODE *find_wnode(GNODE *gfx, int id);

MOB *new_mob(void *ptr, ENV *env, int h, int w, int y0, int x0);
void move_mob(MOB *mob, int dir);

int hit_test(ENV *env, int y, int x);




